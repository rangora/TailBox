#include "Window.h"
#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include <Windowsx.h>

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
            if (tb::Engine::GetDevice() != nullptr && wParam != SIZE_MINIMIZED)
            {
                auto dx12Device = tb::Engine::GetDX12Device();
                dx12Device->WaitForLastSubmittedFrame();
                dx12Device->CleanupRenderTarget();
                HRESULT result = tb::Engine::GetDX12Device()->GetSwapChain()->ResizeBuffers(
                    0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN,
                    DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
                assert(SUCCEEDED(result) && "Failed to resize swapchain.");
                dx12Device->CreateRenderTarget();
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

        _hWnd =
            ::CreateWindowW(_wc.lpszClassName, titleWidStr.c_str(), WS_OVERLAPPEDWINDOW, 100, 100,
                            _windowContext._width, _windowContext._height, nullptr, nullptr, _wc.hInstance, nullptr);

    }

    Window::~Window()
    {
    }

    void Window::Initialize()
    {
        _heapAlloc.Create(Engine::GetDevice(), Engine::GetDX12Device()->GetImGuiDescHeap());

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
        initInfo.Device = Engine::GetDevice();
        initInfo.CommandQueue = Engine::GetDX12Device()->GetCommandQueue();
        initInfo.SrvDescriptorHeap = Engine::GetDX12Device()->GetImGuiDescHeap();
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
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::Render();
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
} // namespace tb
