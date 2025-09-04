#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Graphics/Shader.h"
#include "Graphics/GraphicsCore.h"
#include "Window/Window.h"
#include "Scene/SceneManager.h"
#include <vector>
#include <ShlObj.h>
#include <filesystem>

namespace tb
{
    std::wstring GetPixGpuCapturerPath()
    {
        LPWSTR programFilesPath;
        SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

        std::wstring versionPath;
        std::filesystem::path pixInstalledPath = programFilesPath;
        pixInstalledPath /= "Microsoft PIX";

        for (const auto& directory : std::filesystem::directory_iterator(pixInstalledPath))
        {
            if (directory.is_directory())
            {
                if (versionPath.empty() || versionPath < directory.path().filename().c_str())
                {
                    versionPath = directory.path().filename().c_str();
                }
            }
        }

        if (versionPath.empty())
        {
            return {};
        }

        return pixInstalledPath / versionPath / L"WinPixGpuCapturer.dll";
    }

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
        g_dx12Device.PostDeviceCreated();
        g_commandContext.Initialize();
        g_graphicsResources.Initialize();
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

        _sceneManager->ReleaseAllScenes();
        delete _sceneManager;

        _window->ShutdownImGuiContext();
        _window->ShutdownWindow();
        delete _window;

        g_commandContext.Release();
        g_renderer.Release();
        g_graphicsResources.Release();
        g_dx12Device.ReleaseDevice();
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
        std::filesystem::path pixEventRuntimePath =
            std::filesystem::path(core::enginePath).parent_path() / "external" / "bin" / "x64" / "WinPixEventRuntime.dll";
        HMODULE hPixEventRuntimeModule = LoadLibraryW(pixEventRuntimePath.c_str());
        if (hPixEventRuntimeModule)
        {
            spdlog::info("PixEventRuntime module load success.");
        }

        std::optional<std::wstring> pixCapturerPath = GetPixGpuCapturerPath();
        if (pixCapturerPath)
        {
            HMODULE hCapturererModule = LoadLibraryW(pixCapturerPath.value().c_str());
            if (hCapturererModule)
            {
                spdlog::info("PixGpuCapturerer module load success.");
            }
        }
    }
} // namespace tb