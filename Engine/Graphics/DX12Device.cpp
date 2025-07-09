#include "DX12Device.h"
#include "DescriptorHeap.h"
#include "Engine.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsCore.h"
#include "Scene/SceneManager.h"
#include "Scene/Cube.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "Core/Event.h"
#include "Window/Window.h"

// Windows
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace tb
{
    DX12Device::DX12Device() = default;
    DX12Device::~DX12Device() = default;

    void DX12Device::Initialize()
    {
        timeBeginPeriod(1);
        _lastTime = timeGetTime();

#ifdef _DEBUG
        ::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
        _debugController->EnableDebugLayer();
#endif

        ::CreateDXGIFactory1(IID_PPV_ARGS(&_dxgi));
        ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));

#ifdef _DEBUG
        ID3D12InfoQueue* pInfoQueue = nullptr;
        _device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();

        D3D12_MESSAGE_ID hide[] = {
            D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED
        };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
        filter.DenyList.pIDList = hide;

        pInfoQueue->AddStorageFilterEntries(&filter);
        _debugController.Reset();
#endif
        // Create commandQueue
        {
            D3D12_COMMAND_QUEUE_DESC desc = {};
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask = 1;

            _device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));

            for (int32 i = 0; i < BUFFERCOUNT; ++i)
            {
                _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                IID_PPV_ARGS(&_frameContexts[i]._commandAllocator));
            }
        }

        // Create commandList
        {
            _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _frameContexts[0]._commandAllocator,
                                       nullptr, IID_PPV_ARGS(_commandList.GetAddressOf()));
            _commandList->Close();
            // _rootDescriptorHeap = std::make_unique<DescriptorHeap>(256);

            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            desc.NumDescriptors = 64;
            if (_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_imguiDescHeap.GetAddressOf())) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create imguiDescriptorHeap.");
                return;
            }
        }

        // Create rootDescriptorHeap
        {
            _rootDescriptorHeap = std::make_unique<DescriptorHeap>(_device.Get(), 256);
        }

        // Create fence
        if (_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)) != S_OK)
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
    }

    void DX12Device::CreateSwapChain(const HWND& hWnd)
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
        _swapChain->SetMaximumFrameLatency(BUFFERCOUNT);                        // drop
        _swapChainWaitableObject = _swapChain->GetFrameLatencyWaitableObject(); // drop
    }

    void DX12Device::PostSwapChainCreated()
    {
        if (!_swapChain)
        {
            spdlog::error("[FATAL] Should create swapChain.");
            return;
        }

        // Create RTV
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

        // Create DepthStencil
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = 1;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            if (_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap)) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create dsHeap.");
                return;
            }

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = DXGI_FORMAT_D32_FLOAT;
            clearValue.DepthStencil.Depth = 1.f;
            clearValue.DepthStencil.Stencil = 0.f;

            const uint32 backBufferIndex = _swapChain->GetCurrentBackBufferIndex();
            const D3D12_RESOURCE_DESC rtvDesc = _mainRtvResources[backBufferIndex]->GetDesc();
            _rw = rtvDesc.Width;
            _rh = rtvDesc.Height;

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

        const uint32 backBufferIndex = _swapChain->GetCurrentBackBufferIndex();
        const D3D12_RESOURCE_DESC rtvDesc = _mainRtvResources[backBufferIndex]->GetDesc();
        const uint32 rw = rtvDesc.Width;
        const uint32 rh = rtvDesc.Height;
    }

    void DX12Device::PostDeviceCreated()
    {
        g_renderer.Initialize();
    }

    void DX12Device::ReleaseDevice()
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
        for (int i = 0; i < BUFFERCOUNT; ++i)
        {
            if (_frameContexts[i]._commandAllocator)
            {
                _frameContexts[i]._commandAllocator->Release();
                _frameContexts[i]._commandAllocator = nullptr;
            }
        }
        if (_dxgi)
        {
            _dxgi.Reset();
        }
        if (_dsHeap)
        {
            _dsHeap.Reset();
        }
        if (_commandQueue)
        {
            _commandQueue.Reset();
        }
        if (_commandList)
        {
            _commandList.Reset();
        }
        if (_imguiDescHeap)
        {
            _imguiDescHeap.Reset();
        }
        if (_srvHeap)
        {
            _srvHeap.Reset();
        }
        if (_mainRtvHeap)
        {
            _mainRtvHeap.Reset();
        }
        if (_dsBuffer)
        {
            _dsBuffer.Reset();
        }
        if (_fence)
        {
            _fence.Reset();
        }
        if (_fenceEvent)
        {
            CloseHandle(_fenceEvent);
            _fenceEvent = nullptr;
        }
        if (_device)
        {
            _device.Reset();
        }
#ifdef DX12_ENABLE_DEBUG_LAYER
        IDXGIDebug1* pDebug = nullptr;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
        {
            pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
            pDebug->Release();
        }
#endif
    }

    void DX12Device::Update()
    {
        PreRenderBegin();
        RenderBegin();
        Render();
        RenderEnd();
        PostRenderEnd();

        UpdateTimer();
    }

    void DX12Device::RenderBegin()
    {
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

        _curRootSignature = g_renderer.GetRootSignature("Default")->_rootSignature;

        _commandList->Reset(_nextFrameCtx->_commandAllocator, nullptr);
        //_commandList->SetGraphicsRootSignature(_curRootSignature.Get());

        _rootDescriptorHeap->Clear();

        ID3D12DescriptorHeap* mainHeap = _rootDescriptorHeap->GetDescriptorHeap();
        _commandList->SetDescriptorHeaps(1, &mainHeap);

        _commandList->ResourceBarrier(1, &barrier);

        const float clearColors[4] = {0.45f, 0.55f, 0.6f, 1.f}; // TEMP

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());


        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        if(platform_io.Viewports.Data)
        {
            _viewport.Height = platform_io.Viewports[0]->DrawData->DisplaySize.y;
            _viewport.Width = platform_io.Viewports[0]->DrawData->DisplaySize.x;
            _rect.bottom = platform_io.Viewports[0]->DrawData->DisplaySize.y;
            _rect.right = platform_io.Viewports[0]->DrawData->DisplaySize.x;
        }

        _commandList->RSSetViewports(1, &_viewport);
        _commandList->RSSetScissorRects(1, &_rect);

        _commandList->ClearRenderTargetView(_mainRtvCpuHandle[_backBufferIndex], clearColors, 0, nullptr);
        _commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
    }

    void DX12Device::Render()
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());
        _commandList->OMSetRenderTargets(1, &_mainRtvCpuHandle[_backBufferIndex], FALSE, &dsvHandle);

        _commandList->SetGraphicsRootSignature(_curRootSignature.Get());

        // Render from sceneManager
        Engine::Get().GetSceneManager()->Render();

        // imgui
        RenderImGui();
    }

    void DX12Device::RenderEnd()
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


        HRESULT hr = _swapChain->Present(1, 0); // 1 == vsync
        _bSwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

        uint64 fenceValue = _fenceLastSignalValue + 1;
        _commandQueue->Signal(_fence.Get(), fenceValue);
        _fenceLastSignalValue = fenceValue;
        _nextFrameCtx->_fenceValue = fenceValue;
    }

    void DX12Device::RenderImGui()
    {
        ID3D12DescriptorHeap* descHeaps[] = {_imguiDescHeap.Get()};
        _commandList->SetDescriptorHeaps(_countof(descHeaps), descHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());
    }

    bool DX12Device::IsScreenLocked()
    {
        if (_bSwapChainOccluded && _swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_CLIPPED)
        {
            return true;
        }

        return false;
    }

    void DX12Device::StageBuffer(UploadBuffer* uploadBuffer)
    {
        _staged.emplace_back(uploadBuffer->_resource);
    }

    void DX12Device::CreateRenderTarget()
    {
        for (int i = 0; i < BUFFERCOUNT; ++i)
        {
            ID3D12Resource* pBackBuffer = nullptr;
            _swapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            _device->CreateRenderTargetView(pBackBuffer, nullptr, _mainRtvCpuHandle[i]);
            _mainRtvResources[i] = pBackBuffer;
        }
    }

    void DX12Device::CleanupRenderTarget()
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

    FrameContext* DX12Device::WaitForNextFrameResources()
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

    void DX12Device::WaitForLastSubmittedFrame()
    {
        FrameContext* ctx = &_frameContexts[_frameIndex % BUFFERCOUNT];
        uint64 fenceValue = ctx->_fenceValue;
        if (fenceValue == 0)
        {
            return;
        }

        ctx->_fenceValue = 0;
        if (_fence->GetCompletedValue() >= fenceValue)
        {
            return;
        }

        _fence->SetEventOnCompletion(fenceValue, _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);
    }

    void DX12Device::Flush()
    {
        FrameContext& ctx = _frameContexts[_frameIndex];
        _commandList->Reset(ctx._commandAllocator, nullptr);
    }

    void DX12Device::Signal()
    {
        uint64 fenceValue = _fenceLastSignalValue + 1;
        _commandQueue->Signal(_fence.Get(), fenceValue);
        _fenceLastSignalValue = fenceValue;

        FrameContext& ctx = _frameContexts[_frameIndex];
        ctx._fenceValue = fenceValue;

        _fence->SetEventOnCompletion(fenceValue, _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);
    }

    void DX12Device::PreRenderBegin()
    {
        if (_staged.size())
        {
            Signal();
            Flush();

            _commandList->Close();
            _staged.clear();
        }
    }

    void DX12Device::PostRenderEnd()
    {
        // RenderInfo event
        {
            RenderInfoDisplayEvent event(_currentFPS, _deltaTime);
            EventDispatcher dispatcher(event);

            dispatcher.Dispatch<RenderInfoDisplayEvent>(
                [](RenderInfoDisplayEvent& e)
                {
                    Engine::Get().GetWindow()->OnUpdateRenderTime(e.GetFps(), e.GetDeltaTime());
                    return true;
                });
        }
    }

    void DX12Device::UpdateTimer()
    {
        DWORD now = timeGetTime();
        _deltaTime = (now - _lastTime) * 0.001f; // ms -> s
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
        EventDispatcher disaptcher(event);
        disaptcher.Dispatch<PassDeltaTimeEvent>([](PassDeltaTimeEvent& e)
            {
                Engine::Get().OnUpdateRenderTick(e.GetDeltaTime());
                return true;
            });
    }
} // namespace tb