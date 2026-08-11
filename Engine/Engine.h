#pragma once

#include "Core.h"

#ifdef _WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif

namespace tb
{
    class SceneManager;
    class Window;
    class D3D12RenderAPI;

    class ENGINE_API Engine
    {
    public:
        Engine();
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        ~Engine();

        static Engine& Get() { return *_instance; }
        static SceneManager* GetSceneManager();
        static XMMATRIX GetActiveProjectionMatrix();
        static XMMATRIX GetActiveViewMatrix();
        static Vector3 GetActiveCameraPosition();

        void Launch();
        void EngineTick(const float tick);

    private:
        void LoadModules();
        void UpdateTimer();

        inline static Engine* _instance = nullptr;
        static SceneManager* _sceneManager;

        std::unique_ptr<D3D12RenderAPI> _renderAPI;

        // Timer
        DWORD _lastTime = 0;
        float _deltaTime = 0.f;
        float _fpsTimer = 0.f;
        int32 _frameCount = 0;
        int32 _currentFPS = 0;

        bool _bQuit = false;
    };
}