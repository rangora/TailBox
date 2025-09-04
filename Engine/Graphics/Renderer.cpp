#include "Renderer.h"
#include "InputLayout.h"
#include "PipelineStateHandler.h"
#include "Resources/BaseResource.h"
#include "ShaderCompiler.h"
#include "TextureResource.h"
#include "UploadBuffer.h"
#include "RootSignature.h"
#include "GraphicsCore.h"
#include "MemoryAllocator.h"

namespace tb
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::Initialize()
    {
        _pipelineStateHandler = std::make_unique<PipelineStateHandler>();

        _rootSignatures.reserve(100);

        InitializeRootSignature();
        InitializeShaders();
    }

    ComPtr<ID3D12PipelineState> Renderer::GetPipelineState(const std::string& identifier)
    {
        return _pipelineStateHandler->GetPipelineState(identifier);
    }

    void Renderer::Release()
    {
        for (auto& [_, resource] : _rootSignatures)
        {
            resource.reset();
        }
        _rootSignatures.clear();

        if (_pipelineStateHandler)
        {
            _pipelineStateHandler->Release();
            _pipelineStateHandler.reset();
        }
    }

    void Renderer::InitializeRootSignature()
    {
        // Material
        std::unique_ptr<RootSignature> materialRS = std::make_unique<RootSignature>();
        materialRS->CreateMaterialRootSignature();
        _rootSignatures.emplace("Material", std::move(materialRS));
    }

    void Renderer::InitializeShaders()
    {
        std::vector<ShaderCompileDesc> shaderCompileDescs;
        shaderCompileDescs.push_back({ShaderType::Vertex, tb::core::enginePath + "/Resources/Shader/MaterialVS.hlsl", "Material_VS"});
        shaderCompileDescs.push_back({ShaderType::Pixel, tb::core::enginePath + "/Resources/Shader/MaterialPS.hlsl", "Material_PS"});

        for (const auto& shaderCompileDesc : shaderCompileDescs)
        {
            Shader* newShader = ShaderCompiler::CompileShader(shaderCompileDesc);
            if (newShader != nullptr)
            {
                g_graphicsResources.AddShader(newShader->_identifier, newShader);
            }
        }

        CreateDefaultlPipelineState();
    }

    void Renderer::CreateDefaultlPipelineState()
    {
        for (GraphicsPipelineStateDesc& desc : br::CreateDefaultPipelineState())
        {
            auto VS = g_graphicsResources.GetShader(desc._vsId);
            auto PS = g_graphicsResources.GetShader(desc._psId);

            desc._desc.PS = {PS->_bytecode->GetBufferPointer(), PS->_bytecode->GetBufferSize()};
            desc._desc.VS = {VS->_bytecode->GetBufferPointer(), VS->_bytecode->GetBufferSize()};

            desc._desc.InputLayout = {desc._inputLayout.GetPointer(), static_cast<UINT>(desc._inputLayout.GetSize())};
            desc._desc.pRootSignature = _rootSignatures.find(desc._rootSigantureId)->second->_rootSignature.Get();
            desc._desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            desc._desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            desc._desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            desc._desc.SampleMask = UINT_MAX;
            desc._desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            desc._desc.NumRenderTargets = 1;
            desc._desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc._desc.SampleDesc.Count = 1;
            desc._desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

            _pipelineStateHandler->CreatePipelineState(desc);
        }
    }

}; // namespace tb