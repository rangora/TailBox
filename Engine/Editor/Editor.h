#pragma once

#include "Core.h"
#include "Window/Window.h"

namespace tb
{
    class DX12Device;

    class Editor
    {
    public:
        Editor() = default;
        ~Editor();

        void ShutDown();
        void Update(float delta);

        void CreateWinWindow(const WindowContext& _windowContext);
        void BindDevice(DX12Device* device);
        void CreateDefaultLayout();

        Window* GetWinWindow() const { return _window; } // TEMP
        HWND& GetWndRef() const { return _window->GetWndRef(); }

    private:
        void ProcessKeyInput();

        void TestFunc();
        CD3DX12_CPU_DESCRIPTOR_HANDLE _cpuHandle = {};
        CD3DX12_GPU_DESCRIPTOR_HANDLE _gpuHandle = {};

        Window* _window = nullptr;
    };
} // namespace tb