#pragma once

#include "Core.h"
#include "GpuBuffer.h"
#include "Shader.h"

namespace tb
{
    class PipelineStateHandler;
    class TextureResource;
    class RootSignature;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Initialize();
        GeometryBuffer* GetGeometryBuffer(const std::string& name) { return _geoemtryBuffers.find(name)->second.get(); }
        Shader* GetShader(const std::string& name)                 { return _shaders.find(name)->second.get(); }
        TextureResource* GetTexture(const std::string& name)                 { return _textures.find(name)->second.get(); }
        RootSignature* GetRootSignature(const std::string& name) { return _rootSignatures.find(name)->second.get(); }
        ComPtr<ID3D12PipelineState> GetPipelineState(const std::string& identifier);

    private:
        void InitRootSignature();
        void InitBuffers();
        void InitShaders();

        void CreateMaterialPipelineState();

        std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> _geoemtryBuffers;
        std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;
        std::unordered_map<std::string, std::unique_ptr<TextureResource>> _textures;
        std::unordered_map<std::string, std::unique_ptr<RootSignature>> _rootSignatures;

        std::unique_ptr<PipelineStateHandler> _pipelineStateHandler = nullptr;
    };
} // namespace tb