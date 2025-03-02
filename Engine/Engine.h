#pragma once

#include "Core.h"

namespace tb
{
    class Engine
    {
    public:
        Engine() = default;
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        ~Engine();

        static class DX12Device* GetDX12Device();
        static class ID3D12Device* GetDevice();

        void Launch();
        void Tick(const float tick);

    private:
        static class DX12Device* _DX12device;
        class Window* _window = nullptr;
    };
}