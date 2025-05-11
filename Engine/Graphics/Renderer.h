#pragma once

#include "Core.h"
#include "GpuBuffer.h"
#include "Shader.h"

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
        GeometryBuffer* GetGeometryBuffer(const std::string& name) { return _geoemtryBuffers.find(name)->second.get(); }
        Shader* GetShader(const std::string& name)                 { return _shaders.find(name)->second.get(); }

    private:
        Renderer() = default;
        ~Renderer() = default;

        void InitBuffers();
        void InitShaders();

        std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> _geoemtryBuffers;
        std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;
    };
} // namespace tb