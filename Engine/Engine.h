#pragma once

#include "Core.h"

namespace tb
{
    class Engine
    {
    public:
        Engine();
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        ~Engine();

        static Engine& Get() { return *_instance; }
        static class DX12Device* GetDX12Device();
        static class ID3D12Device* GetDevice();

        void Launch();
        void EngineTick(const float tick);

        class Window* GetWindow() { return _window; }

        void OnUpdateRenderTick(float deltaTime) { _renderTick = deltaTime; }

    private:
        inline static Engine* _instance = nullptr;
        static class DX12Device* _DX12device;
        class Window* _window = nullptr;

        float _renderTick = 0.f;

        bool _bQuit = false;
    };
}