#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/GraphicsCore.h"
#include "Window/Window.h"
#include "Scene/SceneManager.h"
#include <vector>

namespace tb
{
    SceneManager* Engine::_sceneManager = nullptr;

    Engine::Engine()
    {
        _instance = this;
    }

    Engine::~Engine()
    {
    }

    SceneManager* Engine::GetSceneManager()
    {
        return _sceneManager;
    }

    void Engine::Launch()
    {
        _window = new Window({"TailBox", VIEWPORT_WIDTH, VIEWPORT_HEIGHT});
        g_dx12Device.Initialize();
        g_dx12Device.CreateSwapChain(_window->GetWndRef());
        g_dx12Device.PostSwapChainCreated();
        g_dx12Device.PostDeviceCreated();
        _window->Initialize();

        // Init scene.
        _sceneManager = new SceneManager;
        _sceneManager->SetLayer(SceneType::World, "World");

        while (!_bQuit)
        {
            EngineTick(_renderTick);
        }

        // Sutting down
        g_dx12Device.WaitForLastSubmittedFrame();
        _window->ShutdownImGuiContext();
        g_dx12Device.ReleaseDevice();
        _window->ShutdownWindow();
    }

    void Engine::EngineTick(const float tick)
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

        if (g_dx12Device.IsScreenLocked())
        {
            ::Sleep(10);
            return;
        }

        _window->Update(); // key
        _sceneManager->Update(tick); // logic
        _sceneManager->OnRenderBegin();
        g_dx12Device.Update(); // render

        // OnEndFrame
        _sceneManager->OnEndFrame();
    }
} // namespace tb