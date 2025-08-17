#pragma once

#include "Core.h"

namespace tb
{
    static constexpr int BUFFERCOUNT = 2;

    class Renderer;
    class UploadBuffer;
    class CommandContext;

    struct FrameContext
    {
        ID3D12CommandAllocator* _commandAllocator = nullptr;
        uint64 _fenceValue = 0;
    };

    struct StageEntry
    {
        ComPtr<ID3D12Resource> _stagedBuffer = nullptr;
    };

    class DX12Device
    {
        friend CommandContext;
    public:
        DX12Device();
         ~DX12Device();

        void Initialize();
        void CreateSwapChain(const HWND& hWnd);
        void PostSwapChainCreated();
        void PostDeviceCreated();
        void ReleaseDevice();

        void Update();
        void RenderBegin();
        void Render();
        void RenderEnd();
        void RenderImGui(); // 애매..
        bool IsScreenLocked();

        void StageBuffer(UploadBuffer* uploadBuffer);

        ID3D12Device* GetDevice() const { return _device.Get(); }

        IDXGISwapChain3* GetSwapChain() const { return _swapChain.Get(); }
        ID3D12DescriptorHeap* GetImGuiDescHeap() const { return _imguiDescHeap.Get(); }
        ID3D12CommandQueue* GetCommandQueue() const { return _commandQueue.Get(); }
        ID3D12GraphicsCommandList* GetCommmandList() const { return _commandList.Get(); }
        void GetRenderTargetViewSize(float& width, float& height) { width = _rw; height = _rh; }

        void CreateRenderTarget();
        void CleanupRenderTarget();
        FrameContext* WaitForNextFrameResources();
        void WaitForLastSubmittedFrame();

        // 애매한 함수들..
        void Flush();
        void Signal();

    private:
        void PreRenderBegin();
        void PostRenderEnd();

        void UpdateTimer();

        ComPtr<ID3D12Device> _device = nullptr;
        ComPtr<IDXGIFactory4> _dxgi = nullptr;
        ComPtr<ID3D12Debug> _debugController = nullptr;
        ComPtr<ID3D12CommandQueue> _commandQueue = nullptr; // 우선 imgui에 넘겨보자.
        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
        ComPtr<IDXGISwapChain3> _swapChain = nullptr;

        ComPtr<ID3D12DescriptorHeap> _imguiDescHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _dsHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _srvHeap = nullptr;
        ComPtr<ID3D12DescriptorHeap> _mainRtvHeap = nullptr;

        ID3D12Resource* _mainRtvResources[BUFFERCOUNT] = {};
        ComPtr<ID3D12Resource> _dsBuffer = nullptr;

        D3D12_CPU_DESCRIPTOR_HANDLE _mainRtvCpuHandle[BUFFERCOUNT] = {};

        D3D12_VIEWPORT _viewport = {};
        D3D12_RECT _rect = {};
        uint32 _rw = 0;
        uint32 _rh = 0;

        ComPtr<ID3D12Fence> _fence = nullptr;
        HANDLE _fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE _swapChainWaitableObject = nullptr;
        uint64 _fenceLastSignalValue = 0;

        FrameContext _frameContexts[BUFFERCOUNT] = {};
        FrameContext* _nextFrameCtx = nullptr;
        uint32 _frameIndex = 0;
        uint32 _backBufferIndex = 0;
        bool _bSwapChainOccluded = false;

        // time
        DWORD _lastTime = 0;
        float _deltaTime = 0.f;
        float _fpsTimer = 0.f;
        int32 _frameCount = 0;
        int32 _currentFPS = 0;

        std::vector<StageEntry> _staged;
    };
}