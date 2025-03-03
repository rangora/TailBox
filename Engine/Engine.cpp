#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Window/Window.h"

namespace tb
{
    DX12Device* Engine::_DX12device = nullptr;

    Engine::~Engine()
    {
    }

    DX12Device* Engine::GetDX12Device()
    {
        return _DX12device;
    }

    ID3D12Device* Engine::GetDevice()
    {
        return _DX12device->GetDevice();
    }

    void Engine::Launch()
    {
        _DX12device = new DX12Device;
        _window = new Window({"TailBox", 1000, 600});
        _DX12device->CreateSwapChain(_window->GethWndRef());
        _DX12device->PostSwapChainCreated();
        _window->Initialize();

        while (true)
        {
            Tick(0.f);
        }
    }

    void Engine::Tick(const float tick)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
             //   break;
            }
        }

        if (_DX12device->IsScreenLocked())
        {
            ::Sleep(10);
            return;
        }

        _window->Update();
        _DX12device->Update();

        //_window->PostRenderEnd(); // hardcode
    }
} // namespace tb