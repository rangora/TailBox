#include "Window.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/TextureResource.h"
#include "Editor/imgui/imgui_impl_dx12.h"
#include "Editor/imgui/imgui_impl_win32.h"
#include "Core/Input.h"
#include <Windowsx.h>
#include <Editor/EditorCore.h>

// forward declare
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
        case WM_SIZE:
        {
            if (tb::g_dx12Device.GetDevice() != nullptr && wParam != SIZE_MINIMIZED)
            {
                tb::g_dx12Device.OnWindowResized((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
            }
            return 0;
        }

        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);

    return true;
}

namespace tb
{
    DescHeapAllocator Window::_heapAlloc;

    Window::Window(const WindowContext _windowContext)
    : _windowContext(_windowContext)
    {
        std::wstring titleWidStr(_windowContext._title.begin(), _windowContext._title.end());

        _wc = {sizeof(_wc), CS_CLASSDC, WndProc, 0L,      0L,           GetModuleHandle(nullptr),
               nullptr,     nullptr,    nullptr, nullptr, L"TailBox_menu", nullptr};
        ::RegisterClassExW(&_wc);


        DWORD fixedStyle = WS_OVERLAPPED // 기본 윈도우
                           | WS_CAPTION  // 제목 표시줄
                           | WS_SYSMENU  // 시스템 메뉴 (닫기 버튼 등)
                           | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW;

        _hWnd = ::CreateWindowW(_wc.lpszClassName, titleWidStr.c_str(), fixedStyle, 100, 100, _windowContext._width,
                                _windowContext._height, nullptr, nullptr, _wc.hInstance, nullptr);

    }

    Window::~Window()
    {
    }

    void Window::Initialize(DX12Device* device)
    {
        _heapAlloc.Create(device->GetDevice(), g_editor.GetImguiHeap());

        // Seteup Imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.f;
            style.Colors[ImGuiCol_WindowBg].w = 1.f;
        }

        ::ShowWindow(_hWnd, SW_SHOWDEFAULT);
        ::UpdateWindow(_hWnd);

        ImGui_ImplWin32_Init(_hWnd);

        ImGui_ImplDX12_InitInfo initInfo = {};
        initInfo.Device = device->GetDevice();
        initInfo.CommandQueue = device->GetCommandQueue();
        initInfo.SrvDescriptorHeap = g_editor.GetImguiHeap();
        initInfo.NumFramesInFlight = BUFFERCOUNT;
        initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
        initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
                                               D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
        { return Window::_heapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };

        initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
                                              D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
        { return Window::_heapAlloc.Free(cpu_handle, gpu_handle); };

        ImGui_ImplDX12_Init(&initInfo);
    }

    void Window::Update()
    {
        Overlay();
    }

    void Window::PostRenderEnd()
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void Window::ShutdownWindow()
    {
        ::DestroyWindow(_hWnd);
        ::UnregisterClassW(_wc.lpszClassName, _wc.hInstance);
    }

    void Window::OnUpdateRenderTime(float fps, float deltaTime)
    {
        _fps = fps;
        _deltaTime = deltaTime;
    }

    void Window::OnUpdateCameraInfo(const XMFLOAT3& pos, const XMFLOAT3& rot)
    {
        _cameraPos = pos;
        _cameraRot = rot;
    }

    void Window::Overlay()
    {
        static bool bRenderInfo = true;
        static bool bCameraInfo = true;

        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                                        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        const float PAD = 10.f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;

        window_pos.x = work_pos.x + PAD;
        window_pos.y = work_pos.y + PAD;

        window_pos_pivot.x = 0.f;
        window_pos_pivot.y = 0.f;

        if (ImGui::IsMousePosValid())
        {
            _mousePosX = static_cast<int32>(io.MousePos.x);
            _mousePosY = static_cast<int32>(io.MousePos.y);
        }

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowBgAlpha(0.4f);
        if (ImGui::Begin("[Infos]", nullptr, window_flags))
        {
            if (bRenderInfo)
            {
                ImGui::Text("fps:%d, deltaTime:%f", _fps, _deltaTime);
            }
            if (bCameraInfo)
            {
                ImGui::Text("CameraPos:(%0.2f,%0.2f,%0.2f), CameraPot:(%0.2f,%0.2f,%0.2f)", _cameraPos.x, _cameraPos.y,
                            _cameraPos.z, _cameraRot.x, _cameraRot.y, _cameraRot.z);
            }
            ImGui::Separator();

            ImGui::Text("Mouse Position: (%.1f,%.1f)", _mousePosX, _mousePosY);

            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("RenderInfo", nullptr, bRenderInfo == true))
                {
                    bRenderInfo = !bRenderInfo;
                }
                if (ImGui::MenuItem("CameraInfo", nullptr, bCameraInfo == true))
                {
                    bCameraInfo = !bCameraInfo;
                }
                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }
} // namespace tb
