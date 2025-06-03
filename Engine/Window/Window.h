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

        void OnUpdateRenderTime(float fps, float deltaTime);
        void OnUpdateCameraInfo(const XMFLOAT3& pos, const XMFLOAT3& rot);

        HWND& GetWndRef() { return _hWnd; }

        int32 GetMousePosX() const { return _mousePosX; }
        int32 GetMousePosY() const { return _mousePosY; }

        static DescHeapAllocator _heapAlloc;

    private:
        void Overlay();

        // Input
        void ProcessKeyInput();

        WindowContext _windowContext;

        // Window stuff
        WNDCLASSEXW _wc;
        HWND _hWnd;

        // ImGui stuff
        ImGuiContext* _imguiCtx = nullptr;

        int32 _fps = 0;
        float _deltaTime = 0.f;

        XMFLOAT3 _cameraPos = {};
        XMFLOAT3 _cameraRot = {};

        int32 _mousePosX = 0;
        int32 _mousePosY = 0;
    };
}