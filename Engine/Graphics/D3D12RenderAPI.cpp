#include "D3D12RenderAPI.h"
#include "GraphicsCore.h"
#include "Resources/BaseResource.h"
#include "Engine.h"
#include "Editor/EditorCore.h"
#include "Editor/Window/Window.h"
#include "Core/Event.h"
#include "Editor/imgui/imgui.h"
#include "Editor/imgui/imgui_impl_dx12.h"

#include "MemoryAllocator.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace tb
{
    D3D12RenderAPI::D3D12RenderAPI()
    {
#ifdef _DEBUG
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController))))
        {
            _debugController->EnableDebugLayer();
        }

        ID3D12Debug5* debugController5 = nullptr;
        if (S_OK == _debugController->QueryInterface(IID_PPV_ARGS(&debugController5)))
        {
            debugController5->SetEnableGPUBasedValidation(TRUE);
            debugController5->SetEnableAutoName(TRUE);
            debugController5->Release();
        }
        _debugController.Reset();
        _debugController = nullptr;
#endif

        if (!CreateDevice())
        {
            spdlog::error("Failed to create d3d12device.");
            assert(false);
            return;
        }

#ifdef _DEBUG
        ID3D12InfoQueue* pInfoQueue = nullptr;
        _device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        D3D12_MESSAGE_ID hide[] = {D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED};
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
        filter.DenyList.pIDList = hide;

        pInfoQueue->AddStorageFilterEntries(&filter);
        pInfoQueue->Release();
        pInfoQueue = nullptr;
#endif

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        _device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));

        _VOs.resize(1024, nullptr);
        _idxDispenser.Initialize(1024);

         for (int32 i = 0; i < BUFFERCOUNT; ++i)
        {
            _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                            IID_PPV_ARGS(_frameContexts[i]._commandAllocator.GetAddressOf()));
        }

        _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _frameContexts[0]._commandAllocator.Get(),
                                   nullptr, IID_PPV_ARGS(_commandList.GetAddressOf()));
        _commandList->Close();

        _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        IID_PPV_ARGS(_immediateCommandAllocator.GetAddressOf()));
        _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _immediateCommandAllocator.Get(),
                                   nullptr, IID_PPV_ARGS(_immediateCommandList.GetAddressOf()));
        _immediateCommandList->Close();

        _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence2._fence.GetAddressOf()));

        if (_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.GetAddressOf())) != S_OK)
        {
            spdlog::error("[FATAL] Failed to create fence.");
            return;
        }

        // Create fenceEvent
        _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (_fenceEvent == nullptr)
        {
            spdlog::error("[FATAL] Failed to create fenceEvent.");
            return;
        }

        CreateRenderStages();

        timeBeginPeriod(1);
        _lastTime = timeGetTime();

        g_renderAPI = this;
    }

    D3D12RenderAPI::~D3D12RenderAPI()
    {
    }

    void D3D12RenderAPI::SetupStaticMesh(unsigned int& VOI, const std::vector<Vertex>& vertices,
                                         const std::vector<uint32>& indices)
    {
        VOI = AllocateVOIndex();
        D3D12VO* VO = GetVO(VOI);
        if (VO)
        {
            uint32 vertexCount = static_cast<uint32>(br::_cubeVertices.size());
            uint32 vertexBufferSize = vertexCount * sizeof(Vertex);
            VO->_vertexCount = vertexCount;
            VO->_vertexBuffer =
                CreateBuffer(vertexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                             D3D12_HEAP_TYPE_DEFAULT, false, true, br::_cubeVertices.data());
            VO->_vertexBufferView.SizeInBytes = vertexBufferSize;
            VO->_vertexBufferView.StrideInBytes = sizeof(Vertex);
            VO->_vertexBufferView.BufferLocation = VO->_vertexBuffer._gpuAddr;

            uint32 indexCount = static_cast<uint32>(br::_cubeIndices.size());
            uint32 indexBufferSize = static_cast<uint32>(br::_cubeIndices.size() * sizeof(uint32));
            VO->_indexCount = indexCount;
            VO->_indexBuffer =
                CreateBuffer(indexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                            D3D12_HEAP_TYPE_DEFAULT, false, true, br::_cubeIndices.data());
            VO->_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
            VO->_indexBufferView.SizeInBytes = indexBufferSize;
            VO->_indexBufferView.BufferLocation = VO->_indexBuffer._gpuAddr;
        }
    }

    void D3D12RenderAPI::Draw(uint32 VOI, ID3D12GraphicsCommandList* cmdList)
    {
        _drawCommands.emplace_back(VOI);

        cmdList->IASetVertexBuffers(0, 1, &_VOs[VOI]->_vertexBufferView);
        cmdList->IASetIndexBuffer(&_VOs[VOI]->_indexBufferView);
        cmdList->DrawIndexedInstanced(_VOs[VOI]->_indexCount, 1, 0, 0, 0);
    }

    void D3D12RenderAPI::Signal()
    {
        uint64 fenceValue = _fenceLastSignalValue + 1;
        _commandQueue->Signal(_fence.Get(), fenceValue);
        _fenceLastSignalValue = fenceValue;

        _fence->SetEventOnCompletion(fenceValue, _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);
    }

    void D3D12RenderAPI::ExecuteImmediately(std::function<void(ComPtr<ID3D12GraphicsCommandList> cmdList)>&& func)
    {
        _immediateCommandAllocator->Reset();
        _immediateCommandList->Reset(_immediateCommandAllocator.Get(), nullptr);

        func(_immediateCommandList);

        _immediateCommandList->Close();
        ID3D12CommandList* cmdLists[] = {_immediateCommandList.Get()};
        _commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
    
        _fence2._fenceValue++;
        _commandQueue->Signal(_fence2._fence.Get(), _fence2._fenceValue);

        if (_fence2._fence->GetCompletedValue() < _fence2._fenceValue)
        {
            HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
            _fence2._fence->SetEventOnCompletion(_fence2._fenceValue, eventHandle);
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    bool D3D12RenderAPI::CreateDevice()
    {
        CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgi));
        D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
                                             D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};

        IDXGIAdapter1* adapter = nullptr;
        DXGI_ADAPTER_DESC1 adapterDesc = {};
        int32 levelCount = _countof(featureLevels);
        for (int32 i = 0; i < levelCount; ++i)
        {
            int32 adapterIndex = 0;
            while (DXGI_ERROR_NOT_FOUND != _dxgi->EnumAdapters1(adapterIndex, &adapter))
            {
                adapter->GetDesc1(&adapterDesc);
                if (SUCCEEDED(D3D12CreateDevice(adapter, featureLevels[i], IID_PPV_ARGS(&_device))))
                {
                    adapter->Release();
                    adapter = nullptr;
                    return true;
                }

                adapterIndex++;
            }
        }

        if (adapter)
        {
            adapter->Release();
            adapter = nullptr;
        }

        return false;
    }

    void D3D12RenderAPI::CreateSwapChain(const HWND& hWnd)
    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        ZeroMemory(&desc, sizeof(desc));
        desc.BufferCount = BUFFERCOUNT;
        desc.Width = 0;
        desc.Height = 0;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.Stereo = FALSE;

        IDXGISwapChain1* _swapChain1 = nullptr;
        if (_dxgi->CreateSwapChainForHwnd(_commandQueue.Get(), hWnd, &desc, nullptr, nullptr, &_swapChain1) != S_OK)
        {
            spdlog::error("[FATAL] Failed to create swapChain.");
            return;
        }

        _swapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain));
        _swapChain1->Release();
        _dxgi.Reset();
        _swapChain->SetMaximumFrameLatency(BUFFERCOUNT);
        _swapChainWaitableObject = _swapChain->GetFrameLatencyWaitableObject();
    }

    void D3D12RenderAPI::CreateRenderStages()
    {
        for (auto& stage : _renderStages)
        {
            stage._allocators.resize(BUFFERCOUNT);
            stage._commandLists.resize(BUFFERCOUNT);

            for (int32 i = 0; i < BUFFERCOUNT; ++i)
            {
                _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                IID_PPV_ARGS(stage._allocators[i].GetAddressOf()));

                _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                           stage._allocators[i].Get(), nullptr,
                                           IID_PPV_ARGS(stage._commandLists[i].GetAddressOf()));

                stage._commandLists[i]->Close();
            }
        }
    }

    int32 D3D12RenderAPI::AllocateVOIndex()
    {
        int32 idx = _idxDispenser.Allocate();
        if (idx == -1)
        {
            return -1;
        }

        if (!_VOs[idx])
        {
            _VOs[idx] = new D3D12VO;
        }

        return idx;
    }

    D3D12VO* D3D12RenderAPI::GetVO(int32 idx)
    {
        return _VOs[idx];
    }

    void D3D12RenderAPI::FreeVOIndex(int32 idx)
    {
        _idxDispenser.Free(idx);

        if (_VOs[idx])
        {
            delete _VOs[idx];
        }
    }

    D3D12Buffer D3D12RenderAPI::CreateBuffer(uint64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState,
                                             D3D12_HEAP_TYPE heapType, bool cpuAddr, bool gpuAddr, const void* data)
    {
        D3D12Buffer buffer;
        buffer._size = static_cast<uint32>(size);

        /*
        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Alignment = 0;
        bufferDesc.Width = size;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.SampleDesc.Quality = 0;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.Flags = flags;
        */

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

        if (FAILED(_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                    data == nullptr ? initState : D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                    IID_PPV_ARGS(buffer._buffer.GetAddressOf()))))
        {
            spdlog::error("Failed to create default heap buffer.");
            return buffer;
        }

        if (cpuAddr)
        {
            buffer._buffer->Map(0, nullptr, reinterpret_cast<void**>(&buffer._cpuAddr));
        }

        if (gpuAddr)
        {
            buffer._gpuAddr = buffer._buffer->GetGPUVirtualAddress();
        }

        if (data)
        {
            if (cpuAddr)
            {
                memcpy(buffer._cpuAddr, data, size);
            }
            else
            {
                CD3DX12_HEAP_PROPERTIES uploadProp(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
                ComPtr<ID3D12Resource> uploadBuffer = nullptr;
                if (FAILED(_device->CreateCommittedResource(&uploadProp, D3D12_HEAP_FLAG_NONE, &uploadDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                            IID_PPV_ARGS(uploadBuffer.GetAddressOf()))))
                {
                    spdlog::error("Failed to create default heap buffer.");
                    return buffer;
                }

                D3D12_SUBRESOURCE_DATA subData = {};
                subData.pData = data;
                subData.RowPitch = size;
                subData.SlicePitch = size;

                ExecuteImmediately([&](ComPtr<ID3D12GraphicsCommandList> cmdList)
                {
                    UpdateSubresources(cmdList.Get(), buffer._buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subData);
                    CD3DX12_RESOURCE_BARRIER resourceBarrier =
                        CD3DX12_RESOURCE_BARRIER::Transition(buffer._buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                             D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
                    cmdList->ResourceBarrier(1, &resourceBarrier);
                });
            }
        }

        return buffer;
    }

    void D3D12RenderAPI::PostSwapChainCreated()
    {
        if (!_swapChain)
        {
            spdlog::error("[FATAL] Should create swapChain before PostSwapChainCreated.");
            return;
        }

        // RTV
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = BUFFERCOUNT;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 1;

            if (_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_mainRtvHeap)) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create RTVHeap.");
                return;
            }

            uint32 rtvHandleIncrementSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = _mainRtvHeap->GetCPUDescriptorHandleForHeapStart();
            for (int32 i = 0; i < BUFFERCOUNT; ++i)
            {
                _mainRtvCpuHandle[i] = rtvHandle;
                rtvHandle.ptr += rtvHandleIncrementSize;
            }

            CreateRenderTarget();
        }

        // DSV
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = 1;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            if (_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap)) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create dsHeap.");
                return;
            }

            const uint32 backBufferIndex = _swapChain->GetCurrentBackBufferIndex();
            const D3D12_RESOURCE_DESC rtvDesc = _mainRtvResources[backBufferIndex]->GetDesc();
            _rw = rtvDesc.Width;
            _rh = rtvDesc.Height;

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = DXGI_FORMAT_D32_FLOAT;
            clearValue.DepthStencil.Depth = 1.f;
            clearValue.DepthStencil.Stencil = 0.f;

            D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, _rw, _rh, 1, 0, 1, 0,
                                                                               D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            _device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                             D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&_dsBuffer));

            if (_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap)) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create dsHeap after commit.");
                return;
            }
            _dsHeap->SetName(L"depth/stencil resource heap");
            _device->CreateDepthStencilView(_dsBuffer.Get(), &dsvDesc, _dsHeap->GetCPUDescriptorHandleForHeapStart());
        }
    }

    void D3D12RenderAPI::ReleaseDevice()
    {
        CleanupRenderTarget();

        if (_swapChain)
        {
            _swapChain->SetFullscreenState(false, nullptr);
            _swapChain.Reset();
        }
        if (_swapChainWaitableObject != nullptr)
        {
            CloseHandle(_swapChainWaitableObject);
            _swapChainWaitableObject = nullptr;
        }
        if (_dsHeap) _dsHeap.Reset();
        if (_dsBuffer) _dsBuffer.Reset();
        if (_mainRtvHeap) _mainRtvHeap.Reset();
        if (_commandQueue) _commandQueue.Reset();
        if (_commandList) _commandList.Reset();
        if (_fence) _fence.Reset();
        if (_fenceEvent)
        {
            CloseHandle(_fenceEvent);
            _fenceEvent = nullptr;
        }
        if (_device) _device.Reset();

#ifdef _DEBUG
        IDXGIDebug1* pDebug = nullptr;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
        {
            pDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
                                      DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
            pDebug->Release();
        }
#endif
    }


    void D3D12RenderAPI::RenderBegin()
    {
        g_commandContext.Reset();

        _nextFrameCtx = WaitForNextFrameResources();
        _backBufferIndex = _swapChain->GetCurrentBackBufferIndex();
        _nextFrameCtx->_commandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = _mainRtvResources[_backBufferIndex];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        D3D12_RESOURCE_DESC rtvDesc = _mainRtvResources[_backBufferIndex]->GetDesc();
        _rw = rtvDesc.Width;
        _rh = rtvDesc.Height;

        _commandList->Reset(_nextFrameCtx->_commandAllocator.Get(), nullptr);
        _commandList->ResourceBarrier(1, &barrier);

        const float clearColors[4] = {0.45f, 0.55f, 0.6f, 1.f};

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());

        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        if (platform_io.Viewports.Data)
        {
            _viewport.Height = platform_io.Viewports[0]->DrawData->DisplaySize.y;
            _viewport.Width = platform_io.Viewports[0]->DrawData->DisplaySize.x;
            _rect.bottom = static_cast<LONG>(platform_io.Viewports[0]->DrawData->DisplaySize.y);
            _rect.right = static_cast<LONG>(platform_io.Viewports[0]->DrawData->DisplaySize.x);
        }

        _commandList->RSSetViewports(1, &_viewport);
        _commandList->RSSetScissorRects(1, &_rect);
        _commandList->ClearRenderTargetView(_mainRtvCpuHandle[_backBufferIndex], clearColors, 0, nullptr);
        _commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
    }

    void D3D12RenderAPI::Render()
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());
        _commandList->OMSetRenderTargets(1, &_mainRtvCpuHandle[_backBufferIndex], FALSE, &dsvHandle);
        RenderImGui();
    }

    void D3D12RenderAPI::RenderEnd()
    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = _mainRtvResources[_backBufferIndex];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        _commandList->ResourceBarrier(1, &barrier);
        _commandList->Close();

        ID3D12CommandList* ppCommandLists[] = {_commandList.Get()};
        _commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT hr = _swapChain->Present(1, 0);
        _bSwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

        uint64 fenceValue = _fenceLastSignalValue + 1;
        _commandQueue->Signal(_fence.Get(), fenceValue);
        _fenceLastSignalValue = fenceValue;
        _nextFrameCtx->_fenceValue = fenceValue;
    }

    void D3D12RenderAPI::RenderImGui()
    {
        ID3D12DescriptorHeap* descHeaps[] = {g_commandContext._guiDescriptorPool->GetDescriptorHeap()};
        _commandList->SetDescriptorHeaps(_countof(descHeaps), descHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());
    }

    void D3D12RenderAPI::PostRenderEnd()
    {
        RenderInfoDisplayEvent event(_currentFPS, _deltaTime);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<RenderInfoDisplayEvent>(
            [](RenderInfoDisplayEvent& e)
            {
                g_editor.GetWinWindow()->OnUpdateRenderTime(e.GetFps(), e.GetDeltaTime());
                return true;
            });
    }

    void D3D12RenderAPI::UpdateTimer()
    {
        DWORD now = timeGetTime();
        _deltaTime = (now - _lastTime) * 0.001f;
        _lastTime = now;

        _frameCount++;
        _fpsTimer += _deltaTime;
        if (_fpsTimer >= 1.f)
        {
            _currentFPS = _frameCount;
            _frameCount = 0;
            _fpsTimer -= 1.f;
        }

        PassDeltaTimeEvent event(_deltaTime);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<PassDeltaTimeEvent>([](PassDeltaTimeEvent& e)
        {
            Engine::Get().OnUpdateRenderTick(e.GetDeltaTime());
            return true;
        });
    }

    void D3D12RenderAPI::CreateRenderTarget()
    {
        for (int i = 0; i < BUFFERCOUNT; ++i)
        {
            ID3D12Resource* pBackBuffer = nullptr;
            _swapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            _device->CreateRenderTargetView(pBackBuffer, nullptr, _mainRtvCpuHandle[i]);
            _mainRtvResources[i] = pBackBuffer;
        }
    }

    void D3D12RenderAPI::CleanupRenderTarget()
    {
        WaitForLastSubmittedFrame();

        for (int i = 0; i < BUFFERCOUNT; ++i)
        {
            if (_mainRtvResources[i])
            {
                _mainRtvResources[i]->Release();
                _mainRtvResources[i] = nullptr;
            }
        }
    }

    FrameContext* D3D12RenderAPI::WaitForNextFrameResources()
    {
        uint64 nextFrameIndex = _frameIndex + 1;
        _frameIndex = nextFrameIndex;

        HANDLE waitableObjects[] = {_swapChainWaitableObject, nullptr};
        DWORD numWaitableObjects = 1;

        FrameContext* ctx = &_frameContexts[nextFrameIndex % BUFFERCOUNT];
        uint64 fenceValue = ctx->_fenceValue;
        if (fenceValue != 0)
        {
            ctx->_fenceValue = 0;
            _fence->SetEventOnCompletion(fenceValue, _fenceEvent);
            waitableObjects[1] = _fenceEvent;
            numWaitableObjects = 2;
        }

        WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

        return ctx;
    }

    void D3D12RenderAPI::WaitForLastSubmittedFrame()
    {
        FrameContext* ctx = &_frameContexts[_frameIndex % BUFFERCOUNT];
        uint64 fenceValue = ctx->_fenceValue;
        if (fenceValue == 0)
            return;

        ctx->_fenceValue = 0;
        if (_fence->GetCompletedValue() >= fenceValue)
            return;

        _fence->SetEventOnCompletion(fenceValue, _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);
    }

    bool D3D12RenderAPI::IsScreenLocked()
    {
        if (_bSwapChainOccluded && _swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_CLIPPED)
            return true;
        return false;
    }

    void D3D12RenderAPI::OnWindowResized(UINT width, UINT height)
    {
        WaitForLastSubmittedFrame();
        CleanupRenderTarget();
        HRESULT result = _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN,
                                                   DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
        assert(SUCCEEDED(result) && "Failed to resize swapchain.");
        CreateRenderTarget();

        _dsBuffer.Reset();

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.f;
        clearValue.DepthStencil.Stencil = 0.f;

        D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1,
                                                                           0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        _device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                         D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&_dsBuffer));

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

        _device->CreateDepthStencilView(_dsBuffer.Get(), &dsvDesc, _dsHeap->GetCPUDescriptorHandleForHeapStart());
    }

} // namespace tb