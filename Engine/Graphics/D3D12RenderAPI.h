#pragma once

#include "RenderAPI.h"
#include "DirectX12/D3D12Struct.h"
#include "Utility/IndexDispenser.h"
#include "RenderEnum.h"

namespace tb
{
    static constexpr int BUFFERCOUNT = 2;

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

        // Init / Shutdown
        void CreateSwapChain(const HWND& hWnd);
        void PostSwapChainCreated();
        void ReleaseDevice();

        // Frame loop
        void BeginFrame();
        void EndFrame();
        void PostRenderEnd();
        void UpdateTimer();
        bool IsScreenLocked();
        void WaitForLastSubmittedFrame();

        D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const { return _mainRtvCpuHandle[_backBufferIndex]; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return _dsHeap->GetCPUDescriptorHandleForHeapStart(); }

        // Window
        void OnWindowResized(UINT width, UINT height);
        void GetRenderTargetViewSize(float& width, float& height) { width = static_cast<float>(_rw); height = static_cast<float>(_rh); }

        IDXGISwapChain3* GetSwapChain() const { return _swapChain.Get(); }
        ID3D12CommandQueue* GetCommandQueue() const { return _commandQueue.Get(); }
        ID3D12Device* GetDevice() const { return _device.Get(); }

        void ExecuteImmediately(std::function<void(ComPtr<ID3D12GraphicsCommandList> cmdList)>&& func);
        void Signal();

        ComPtr<ID3D12Device> _device = nullptr;

    private:
        bool CreateDevice();
        void CreateRenderTarget();
        void CleanupRenderTarget();
        void CreateRenderStages();
        FrameContext* WaitForNextFrameResources();


        int32 AllocateVOIndex();
        D3D12VO* GetVO(int32 idx);
        void FreeVOIndex(int32 idx);

        D3D12Buffer CreateBuffer(uint64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState,
                                 D3D12_HEAP_TYPE heapType, bool cpuAddr, bool gpuAddr, const void* data);

        std::vector<D3D12VO*> _VOs;
        std::vector<D3D12DrawCommand> _drawCommands;
        std::array<D3D12RenderStage, RenderStageType::REDNERSTAGE_MAX> _renderStages;

        ComPtr<ID3D12CommandQueue> _commandQueue = nullptr;
        IndexDispenser _idxDispenser;

        FrameContext _frameContexts[2] = {}; // BUFFERCOUNT
        FrameContext* _nextFrameCtx = nullptr;

        ComPtr<IDXGIFactory4> _dxgi = nullptr;
        ComPtr<IDXGISwapChain3> _swapChain = nullptr;
        ComPtr<ID3D12Debug> _debugController = nullptr;
        ComPtr<ID3D12Fence> _fence = nullptr;

        HANDLE _fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE _swapChainWaitableObject = nullptr;
        uint64 _fenceLastSignalValue = 0;

        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
        ComPtr<ID3D12CommandAllocator> _immediateCommandAllocator = nullptr;
        ComPtr<ID3D12GraphicsCommandList> _immediateCommandList = nullptr;
        D3D12Fence _fence2;

        // SwapChain / RTV
        ComPtr<ID3D12DescriptorHeap> _mainRtvHeap = nullptr;
        ID3D12Resource* _mainRtvResources[2] = {}; // BUFFERCOUNT
        D3D12_CPU_DESCRIPTOR_HANDLE _mainRtvCpuHandle[2] = {}; // BUFFERCOUNT

        // DSV
        ComPtr<ID3D12DescriptorHeap> _dsHeap = nullptr;
        ComPtr<ID3D12Resource> _dsBuffer = nullptr;

        uint32 _rw = 0;
        uint32 _rh = 0;

        uint32 _frameIndex = 0;
        uint32 _backBufferIndex = 0;
        bool _bSwapChainOccluded = false;

        // Timer
        DWORD _lastTime = 0;
        float _deltaTime = 0.f;
        float _fpsTimer = 0.f;
        int32 _frameCount = 0;
        int32 _currentFPS = 0;
    };
}
