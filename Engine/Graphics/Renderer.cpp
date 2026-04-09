#include "Renderer.h"
#include "InputLayout.h"
#include "PipelineStateHandler.h"
#include "Resources/BaseResource.h"
#include "ShaderCompiler.h"
#include "TextureResource.h"
#include "UploadBuffer.h"
#include "GraphicsCore.h"
#include "MemoryAllocator.h"
#include "RenderPassManager.h"
#include "RenderAPI.h"
#include "../Engine.h"

namespace tb
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::Initialize()
    {
        RenderAPI::Create();
        RenderPassManager::Create();

        RenderPassManager::Get()->SetupRenderPass(RenderPiplineType::Forward);

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
        for (auto& [_, rs] : _rootSignatures)
        {
            rs.Reset();
        }
        _rootSignatures.clear();

        if (_pipelineStateHandler)
        {
            _pipelineStateHandler->Release();
            _pipelineStateHandler.reset();
        }
    }

    void Renderer::RenderBegin()
    {

    }

    void Renderer::Render()
    {
        // view 정보 업데이트 하는거 개선 해여 함
        auto projMtx = Engine::GetActiveProjectionMatrix();
        auto viewMtx = Engine::GetActiveViewMatrix();

        D3D12View view{projMtx, viewMtx};
        auto renderPassMgr = RenderPassManager::Get();
        renderPassMgr->Render(view);
    }

    void Renderer::InitializeRootSignature()
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

        CD3DX12_ROOT_SIGNATURE_DESC desc;
        desc.Init(_countof(params), params, 1, &sampler, flags);

        ComPtr<ID3DBlob> blobSignature;
        ComPtr<ID3DBlob> blobError;
        if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError)))
        {
            assert(false);
            return;
        }

        ComPtr<ID3D12RootSignature> rs;
        g_dx12Device.GetDevice()->CreateRootSignature(
            0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(),
            IID_PPV_ARGS(rs.GetAddressOf()));

        _rootSignatures.emplace("Material", std::move(rs));
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
            desc._desc.pRootSignature = _rootSignatures.find(desc._rootSigantureId)->second.Get();
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