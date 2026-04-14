#pragma once

//#include "Core.h"
#include "GpuBuffer.h"
#include "Shader.h"
#include "DirectX12/D3D12Struct.h"

namespace tb
{
    class TextureResource;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Initialize();

        ID3D12RootSignature* GetRootSignature(const std::string& name);
        ComPtr<ID3D12PipelineState> GetPipelineState(const std::string& name);

        void Release();

        void RenderBegin();
        void Render();

    private:
        void InitializePipelines();
        void InitializeShaders();

        std::unordered_map<std::string, D3D12Pipeline> _pipelines;
    };
} // namespace tb