#pragma once

#include "Core.h"
#include "GpuBuffer.h"

namespace tb
{

    class Renderer
    {
    public:
        static Renderer* Get()
        {
            static Renderer instance;
            return &instance;
        }

        void Initialize();
        GeometryBuffer* GetGeometryBuffer(const std::string& name);

    private:
        Renderer() = default;
        ~Renderer() = default;

        ID3D12Device* _device = nullptr;

        std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> _geoemtryBuffers;
    };
} // namespace tb