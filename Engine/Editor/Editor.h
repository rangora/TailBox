#pragma once

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

        Window* GetWinWindow() { return _window; } // TEMP

        HWND& GetWndRef() { return _window->GetWndRef(); }

    private:
        Window* _window = nullptr;
    };
} // namespace tb