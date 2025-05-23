#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Window/Window.h"
#include "Scene/SceneManager.h"
#include <vector>

// TEMP
#include "Scene/Actor.h"

namespace tb
{
    DX12Device* Engine::_DX12device = nullptr;

    Engine::Engine()
    {
        _instance = this;
    }

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
        _DX12device->CreateSwapChain(_window->GetWndRef());
        _DX12device->PostSwapChainCreated();
        _DX12device->PostDeviceCreated();
        _window->Initialize();

        TEMP_Initial();

        while (!_bQuit)
        {
            Tick(0.f);
        }

        // Sutting down
        _DX12device->WaitForLastSubmittedFrame();
        _window->ShutdownImGuiContext();
        _DX12device->ReleaseDevice();
        _window->ShutdownWindow();
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
                _bQuit = true;
                break;
            }
        }

        if (_bQuit)
        {
            return;
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

    void Engine::TEMP_Initial()
    {
        // Scene
        SceneManager* sceneMgr = SceneManager::Get();
        sceneMgr->SetLayer(SceneType::World, "World");

        _mesh = new Mesh();
    }
} // namespace tb