#include "D3D12RenderAPI.h"
#include "GraphicsCore.h"
#include "Resources/BaseResource.h"

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
            VO->_vertexBuffer = CreateBuffer(vertexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                             D3D12_HEAP_TYPE_DEFAULT, false, true, br::_cubeVertices.data());
            VO->_vertexBufferView.SizeInBytes = vertexBufferSize;
            VO->_vertexBufferView.StrideInBytes = sizeof(Vertex);
            VO->_vertexBufferView.BufferLocation = VO->_vertexBuffer._gpuAddr;

            uint32 indexCount = static_cast<uint32>(br::_cubeIndices.size());
            uint32 indexBufferSize = static_cast<uint32>(br::_cubeIndices.size() * sizeof(uint32));
            VO->_indexCount = indexCount;
            VO->_indexBuffer = CreateBuffer(indexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
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
        //_swapChain->SetMaximumFrameLatency(BUFFERCOUNT);                        // drop
        //_swapChainWaitableObject = _swapChain->GetFrameLatencyWaitableObject(); // drop
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

                // 대체 필
                 g_dx12Device.Flush();
                UpdateSubresources(g_dx12Device.GetCommmandList(), buffer._buffer.Get(), uploadBuffer.Get(), 0, 0,
                                   1, &subData);

                CD3DX12_RESOURCE_BARRIER resourceBarrier =
                    CD3DX12_RESOURCE_BARRIER::Transition(buffer._buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

                g_dx12Device.GetCommmandList()->ResourceBarrier(1, &resourceBarrier);

                g_dx12Device.GetCommmandList()->Close();
                ID3D12CommandList* ppCommandLists[] = {g_dx12Device.GetCommmandList()};
                g_dx12Device.GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);
                g_dx12Device.Signal();
                // 대체
            }
        }

        return buffer;
    }

} // namespace tb