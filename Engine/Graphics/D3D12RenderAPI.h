#pragma once

#include "RenderAPI.h"
#include "DirectX12/D3D12Struct.h"
#include "Utility/IndexDispenser.h"
#include <vector>

namespace tb
{
    struct FrameContext
    {
        ComPtr<ID3D12CommandAllocator> _commandAllocator = nullptr;
        uint64 _fenceValue = 0;
    };

    class D3D12RenderAPI : public RenderAPI
    {
    public:
        D3D12RenderAPI();
        ~D3D12RenderAPI();

        void SetupStaticMesh(unsigned int& VOI, const std::vector<Vertex>& vertices, const std::vector<uint32>& indices) final;

        void Draw(uint32 VOI, ID3D12GraphicsCommandList* cmdList) final;

        void Signal();

        ID3D12CommandQueue* GetCommandQueue() const { return _commandQueue.Get(); }

        ComPtr<ID3D12Device> _device = nullptr;

    private:
        bool CreateDevice();
        void CreateSwapChain(const HWND& hWnd);

        int32 AllocateVOIndex();
        D3D12VO* GetVO(int32 idx);
        void FreeVOIndex(int32 idx);

        D3D12Buffer CreateBuffer(uint64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState,
                                 D3D12_HEAP_TYPE heapType, bool cpuAddr, bool gpuAddr, const void* data);

        std::vector<D3D12VO*> _VOs;
        std::vector<D3D12DrawCommand> _drawCommands;

        ComPtr<ID3D12CommandQueue> _commandQueue = nullptr;

        IndexDispenser _idxDispenser;

        FrameContext _frameContexts[2] = {}; // BUFFERCOUNT
        ComPtr<ID3D12Device> _dx12Device = nullptr;
        ComPtr<IDXGIFactory4> _dxgi = nullptr;
        ComPtr<IDXGISwapChain3> _swapChain = nullptr;
        ComPtr<ID3D12Debug> _debugController = nullptr;
        ComPtr<ID3D12Fence> _fence = nullptr;
        
        HANDLE _fenceEvent = INVALID_HANDLE_VALUE;
        uint64 _fenceLastSignalValue = 0;

        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
        ComPtr<ID3D12CommandAllocator> _commandAllocator[2]; // ?
    };
}