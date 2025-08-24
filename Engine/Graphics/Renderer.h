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
        RootSignature* GetRootSignature(const std::string& name) { return _rootSignatures.find(name)->second.get(); }
        ComPtr<ID3D12PipelineState> GetPipelineState(const std::string& identifier);

        void Release();

    private:
        void InitializeRootSignature();
        void InitializeShaders();

        void CreateDefaultlPipelineState();

        std::unordered_map<std::string, std::unique_ptr<RootSignature>> _rootSignatures;
        std::unique_ptr<PipelineStateHandler> _pipelineStateHandler = nullptr;
    };
} // namespace tb