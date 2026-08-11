#include "Engine.h"
#include "Graphics/Shader.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/D3D12RenderAPI.h"
#include "Graphics/EditorRenderer.h"
#include "Editor/Window/Window.h"
#include "Editor/EditorCore.h"
#include "Editor/imgui/imgui.h"
#include "Scene/SceneManager.h"
#include <vector>
#include <ShlObj.h>
#include <filesystem>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

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

        g_editor.CreateWinWindow({"TailBox", VIEWPORT_WIDTH, VIEWPORT_HEIGHT});

        _renderAPI = std::make_unique<D3D12RenderAPI>(); // 생성자 내부에서 g_renderAPI = this로 자기 등록
        timeBeginPeriod(1);
        _lastTime = timeGetTime();

        g_renderer.Initialize(); // RenderPassManager 등 초기화 (D3D12RenderAPI는 Engine이 이미 생성)

        _renderAPI->CreateSwapChain(g_editor.GetWndRef());
        _renderAPI->PostSwapChainCreated();

        g_commandContext.Initialize();
        g_graphicsResources.Initialize();

        g_editor.BindDevice();

        // Init scene.
        _sceneManager = new SceneManager;
        _sceneManager->SetLayer(SceneType::World, "World");

        while (!_bQuit)
        {
            EngineTick(_deltaTime);
        }

        // Sutting down
        _renderAPI->WaitForLastSubmittedFrame();

        _sceneManager->ReleaseAllScenes();
        delete _sceneManager;

        g_commandContext.Release();
        g_graphicsResources.Release();
        g_renderer.Release();
        g_editor.ShutDown();
        _renderAPI->ReleaseDevice();
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

        if (_renderAPI->IsScreenLocked())
        {
            ::Sleep(10);
            return;
        }

        g_editor.Update(tick);
        _sceneManager->Update(tick);

        _renderAPI->BeginFrame();
        _sceneManager->OnRenderBegin();

        g_editor.RenderScene();
        g_renderer.Render();
        EditorRenderer::Get()->Render();

        _sceneManager->OnEndFrame();
        _renderAPI->EndFrame();

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        _renderAPI->Present();

        UpdateTimer();
    }

    void Engine::UpdateTimer()
    {
        DWORD now = timeGetTime();
        _deltaTime = (now - _lastTime) * 0.001f;
        _lastTime = now;

        _frameCount++;
        _fpsTimer += _deltaTime;
        if (_fpsTimer >= 1.f)
        {
            _currentFPS = _frameCount;
            _frameCount = 0;
            _fpsTimer -= 1.f;
        }

        g_editor.GetWinWindow()->OnUpdateRenderTime(_currentFPS, _deltaTime);
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