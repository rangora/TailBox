#pragma once

#include "Core.h"
#include "Window/Window.h"

namespace tb
{
    class Editor
    {
    public:
        Editor();
        ~Editor();

        void ShutDown();
        void Update(float delta);

        void CreateWinWindow(const WindowContext& _windowContext);
        void BindDevice();
        void CreateDefaultLayout();

        Window* GetWinWindow() const { return _window; } // TEMP
        HWND& GetWndRef() const { return _window->GetWndRef(); }

    private:
        void ProcessKeyInput();

        void TestFunc2();
        CD3DX12_CPU_DESCRIPTOR_HANDLE _cpuHandle = {};
        CD3DX12_GPU_DESCRIPTOR_HANDLE _gpuHandle = {};

        ComPtr<ID3D12Resource> _rtvResource = nullptr;
        ComPtr<ID3D12DescriptorHeap> _textureRtvHeap = nullptr;
        CD3DX12_CPU_DESCRIPTOR_HANDLE _rtvHandle = {};

        std::unique_ptr<class RenderTexture> _renderTexture = nullptr;

        Window* _window = nullptr;
    };
} // namespace tb