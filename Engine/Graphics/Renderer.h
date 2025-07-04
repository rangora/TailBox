#pragma once

#include "Core.h"
#include "GpuBuffer.h"
#include "Shader.h"

namespace tb
{
    class PipelineStateHandler;
    class Texture;

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
        Texture* GetTexture(const std::string& name)                 { return _textures.find(name)->second.get(); }
        ComPtr<ID3D12PipelineState> GetPipelineState(const std::string& identifier);

    private:
        Renderer();
        ~Renderer();

        void InitBuffers();
        void InitShaders();

        std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> _geoemtryBuffers;
        std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;
        std::unordered_map<std::string, std::unique_ptr<Texture>> _textures;

        std::unique_ptr<PipelineStateHandler> _pipelineStateHandler = nullptr;
    };
} // namespace tb