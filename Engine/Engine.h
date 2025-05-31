#pragma once

#include "Core.h"

namespace tb
{
    class Mesh;
    class Cube;

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
        void Tick(const float tick);

        class Window* GetWindow() { return _window; }

        Mesh* _mesh = nullptr;
        Cube* _cube = nullptr;

    private:

        void TEMP_Initial();

        inline static Engine* _instance = nullptr;
        static class DX12Device* _DX12device;
        class Window* _window = nullptr;

        bool _bQuit = false;
    };
}