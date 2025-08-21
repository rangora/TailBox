#include "Engine.h"
#include "Graphics/DX12Device.h"
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

    XMMATRIX Engine::GetActiveProjectionMatrix()
    {
        return GetSceneManager()->GetProjectionMatrix();
    }

    XMMATRIX Engine::GetActiveViewMatrix()
    {
        return GetSceneManager()->GetViewMatrix();
    }

    Vector3 Engine::GetActiveCameraPosition()
    {
        return GetSceneManager()->GetCameraPosition();
    }

    void Engine::Launch()
    {
        LoadModules();

        _window = new Window({"TailBox", VIEWPORT_WIDTH, VIEWPORT_HEIGHT});
        g_dx12Device.Initialize();
        g_dx12Device.CreateSwapChain(_window->GetWndRef());
        g_dx12Device.PostSwapChainCreated();
        g_commandContext.Initialize();
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

    void Engine::LoadModules()
    {
        std::string pix = "C:/Program Files/Microsoft PIX/2405.15.002-OneBranch_release/WinPixGpuCapturer.dll";
        HMODULE hPixModule = LoadLibraryA(pix.c_str());
        if (hPixModule)
        {
            spdlog::info("pix module load success.");
        }
    }
} // namespace tb