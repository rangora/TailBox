#pragma once

#include "Core.h"
#include "GpuBuffer.h"
#include "Shader.h"

namespace tb
{
    class PipelineStateHandler;
    class TextureResource;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Initialize();
        ID3D12RootSignature* GetRootSignature(const std::string& name)
        {
            auto it = _rootSignatures.find(name);
            return (it != _rootSignatures.end()) ? it->second.Get() : nullptr;
        }
        ComPtr<ID3D12PipelineState> GetPipelineState(const std::string& identifier);

        void Release();

        void RenderBegin();
        void Render();

    private:
        void InitializeRootSignature();
        void InitializeShaders();

        void CreateDefaultlPipelineState();

        std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> _rootSignatures;
        std::unique_ptr<PipelineStateHandler> _pipelineStateHandler = nullptr;
    };
} // namespace tb