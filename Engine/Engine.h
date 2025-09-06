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

        void OnUpdateRenderTick(float deltaTime) { _renderTick = deltaTime; }

    private:
        void LoadModules();

        inline static Engine* _instance = nullptr;
        static SceneManager* _sceneManager;

        float _renderTick = 0.f;

        bool _bQuit = false;
    };
}