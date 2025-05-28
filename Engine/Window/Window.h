#pragma once

#include "Core.h"
#include "DescHeapAllocator.h"

struct ImGuiContext;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace tb
{
    struct WindowContext
    {
        std::string _title;
        int32 _width = 0;
        int32 _height = 0;

        WindowContext(const std::string& title = "NewWindow", int32 width = 1000, int32 height = 600)
            : _title(title), _width(width), _height(height) {}
    };

    class Window
    {
    public:
        Window() = delete;
        Window(const WindowContext winContext);
        ~Window();

        void Initialize();
        void Update();
        void PostRenderEnd();

        void ShutdownImGuiContext();
        void ShutdownWindow();

        HWND& GetWndRef() { return _hWnd; }

        static DescHeapAllocator _heapAlloc;

    private:
        void Overlay();

        WindowContext _windowContext;

        // Window stuff
        WNDCLASSEXW _wc;
        HWND _hWnd;

        // ImGui stuff
        ImGuiContext* _imguiCtx = nullptr;
    };
}