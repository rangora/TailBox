#pragma once

#include "Core.h"

namespace tb
{
    class SceneManager;
    class DX12Device;
    class Window;

    class Engine
    {
    public:
        Engine();
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        ~Engine();

        static Engine& Get() { return *_instance; }
        static DX12Device* GetDX12Device();
        static ID3D12Device* GetDevice();
        static SceneManager* GetSceneManager();

        void Launch();
        void EngineTick(const float tick);

        class Window* GetWindow() { return _window; }

        void OnUpdateRenderTick(float deltaTime) { _renderTick = deltaTime; }

    private:
        inline static Engine* _instance = nullptr;
        Window* _window = nullptr;
        static DX12Device* _DX12device;
        static SceneManager* _sceneManager;

        float _renderTick = 0.f;

        bool _bQuit = false;
    };
}