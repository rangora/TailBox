#include "Renderer.h"
#include "Engine.h"
#include "InputLayout.h"
#include "Resources/BaseResource.h"
#include "ShaderCompiler.h"
#include "TextureResource.h"
#include "GraphicsCore.h"
#include "RenderPassManager.h"
#include "RenderAPI.h"

namespace tb
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::Initialize()
    {
        RenderAPI::Create();
        RenderPassManager::Create();

        RenderPassManager::Get()->SetupRenderPass(RenderPiplineType::Forward);

        _pipelines.reserve(100);

        InitializePipelines();
        InitializeShaders();
    }

    ID3D12RootSignature* Renderer::GetRootSignature(const std::string& name)
    {
        auto it = _pipelines.find(name);
        return (it != _pipelines.end()) ? it->second.rootSignature.Get() : nullptr;
    }

    ComPtr<ID3D12PipelineState> Renderer::GetPipelineState(const std::string& name)
    {
        auto it = _pipelines.find(name);
        return (it != _pipelines.end()) ? it->second.pipelineState : nullptr;
    }

    void Renderer::Release()
    {
        for (auto& [_, pipeline] : _pipelines)
        {
            pipeline.rootSignature.Reset();
            pipeline.pipelineState.Reset();
        }
        _pipelines.clear();
    }

    void Renderer::RenderBegin()
    {

    }

    void Renderer::Render()
    {
        // view 정보 업데이트 하는거 개선 해여 함
        XMMATRIX projMtx = Engine::GetActiveProjectionMatrix();
        XMMATRIX viewMtx = Engine::GetActiveViewMatrix();

        D3D12View view{projMtx, viewMtx};
        auto renderPassMgr = RenderPassManager::Get();
        renderPassMgr->Render(view);
    }

    void Renderer::InitializePipelines()
    {
        // Material
        D3D12Pipeline material;

        // RootSignature
        {
            CD3DX12_DESCRIPTOR_RANGE srvRange = {};
            srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

            CD3DX12_ROOT_PARAMETER params[2] = {};
            params[0].InitAsConstantBufferView(0);
            params[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

            D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);

            D3D12_ROOT_SIGNATURE_FLAGS flags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
            rsDesc.Init(_countof(params), params, 1, &sampler, flags);

            ComPtr<ID3DBlob> blobSignature;
            ComPtr<ID3DBlob> blobError;
            if (FAILED(D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError)))
            {
                assert(false);
                return;
            }

            g_dx12Device.GetDevice()->CreateRootSignature(
                0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(),
                IID_PPV_ARGS(material.rootSignature.GetAddressOf()));
        }

        _pipelines.emplace("Material", std::move(material));
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

        // Material PSO
        {
            auto& material = _pipelines.at("Material");

            auto VS = g_graphicsResources.GetShader("Material_VS");
            auto PS = g_graphicsResources.GetShader("Material_PS");
            InputLayout inputLayout = InputLayoutPreset::MaterialInputLayout();

            D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
            desc.pRootSignature = material.rootSignature.Get();
            desc.VS = {VS->_bytecode->GetBufferPointer(), VS->_bytecode->GetBufferSize()};
            desc.PS = {PS->_bytecode->GetBufferPointer(), PS->_bytecode->GetBufferSize()};
            desc.InputLayout = {inputLayout.GetPointer(), static_cast<UINT>(inputLayout.GetSize())};
            desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            desc.SampleMask = UINT_MAX;
            desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            desc.NumRenderTargets = 1;
            desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

            if (FAILED(g_dx12Device.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(material.pipelineState.GetAddressOf()))))
            {
                spdlog::error("Failed to create Material pipeline state");
                assert(false);
            }
        }
    }

}; // namespace tb
