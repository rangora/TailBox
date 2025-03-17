#pragma once

#include "Core.h"

namespace tb
{
    static constexpr int BUFFERCOUNT = 2;

    class DescriptorHeap;

    struct FrameContext
    {
        ID3D12CommandAllocator* _commandAllocator = nullptr;
        uint64 _fenceValue = 0;
    };

    class DX12Device
    {
    public:
        DX12Device();
         ~DX12Device();

        void CreateSwapChain(const HWND& hWnd);
        void PostSwapChainCreated();
        void ReleaseDevice();

        void Update();
        void RenderBegin();
        void Render();
        void RenderEnd();
        void RenderImGui(); // 애매..
        bool IsScreenLocked();

        ID3D12Device* GetDevice() const { return _device.Get(); }

        IDXGISwapChain3* GetSwapChain() const { return _swapChain.Get(); }
        ID3D12DescriptorHeap* GetImGuiDescHeap() const { return _imguiDescHeap.Get(); }
        ID3D12CommandQueue* GetCommandQueue() const { return _commandQueue.Get(); }
        ID3D12GraphicsCommandList* GetCommmandList() const { return _commandList.Get(); }
        ID3D12RootSignature* GetRootSignature() const { return _rootSignature.Get(); }
        DescriptorHeap* GetRootDescriptorHeap() const { return _rootDescriptorHeap.get(); }

        void CreateRenderTarget();
        void CleanupRenderTarget();
        FrameContext* WaitForNextFrameResources();
        void WaitForLastSubmittedFrame();

    private:
        ComPtr<ID3D12Device> _device = nullptr;
        ComPtr<IDXGIFactory4> _dxgi = nullptr;
        ComPtr<ID3D12Debug> _debugController = nullptr;
        ComPtr<ID3D12CommandQueue> _commandQueue = nullptr; // 우선 imgui에 넘겨보자.
        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
        ComPtr<ID3D12GraphicsCommandList> _graphicsCommandList = nullptr;
        ComPtr<IDXGISwapChain3> _swapChain = nullptr;

        ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

        std::unique_ptr<DescriptorHeap> _rootDescriptorHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _imguiDescHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _dsHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _srvHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _mainRtvHeap = nullptr;

        ID3D12Resource* _mainRtvResources[BUFFERCOUNT] = {};
        ComPtr<ID3D12Resource> _dsBuffer = nullptr;

        D3D12_CPU_DESCRIPTOR_HANDLE _mainRtvCpuHandle[BUFFERCOUNT] = {};

        D3D12_VIEWPORT _viewport;
        D3D12_RECT _rect;

        ComPtr<ID3D12Fence> _fence = nullptr;
        HANDLE _fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE _swapChainWaitableObject = nullptr;
        uint32 _fenceLastSignalValue = 0;

        FrameContext _frameContexts[BUFFERCOUNT] = {};
        FrameContext* _nextFrameCtx = nullptr;
        uint32 _frameIndex = 0;
        uint32 _backBufferIndex = 0;
        bool _bSwapChainOccluded = false;
    };
}