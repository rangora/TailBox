#include "Renderer.h"
#include "DX12Device.h"
#include "Engine.h"
#include "InputLayout.h"
#include "PipelineStateHandler.h"
#include "Resources/BaseResource.h"
#include "ShaderCompiler.h"
#include "UploadBuffer.h"

namespace tb
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::Initialize()
    {
        _pipelineStateHandler = std::make_unique<PipelineStateHandler>();

        _geoemtryBuffers.reserve(100);
        _shaders.reserve(100);

        InitBuffers();
        InitShaders();

    }

    ComPtr<ID3D12PipelineState> Renderer::GetPipelineState(const std::string& identifier)
    {
        return _pipelineStateHandler->GetPipelineState(identifier);
    }

    void Renderer::InitBuffers()
    {
        _geoemtryBuffers.emplace("Box", std::move(br::CreateBoxBuffer()));
        _geoemtryBuffers.emplace("Cube", std::move(br::CreateCubeBuffer()));
    }

    void Renderer::InitShaders()
    {
        std::vector<ShaderCompileDesc> shaderCompileDescs;
        shaderCompileDescs.push_back({ShaderType::Vertex, tb::core::projectPath + "/Resources/Shader/BaseVS.hlsl", "Cube_VS"});
        shaderCompileDescs.push_back({ShaderType::Pixel, tb::core::projectPath + "/Resources/Shader/BasePS.hlsl", "Cube_PS"});

        for (const auto& shaderCompileDesc : shaderCompileDescs)
        {
            Shader* newShader = ShaderCompiler::CompileShader(shaderCompileDesc);
            if (newShader != nullptr)
            {
                _shaders.emplace(newShader->_identifier, newShader);
            }
        }

        GraphicsPipelineStateDesc pipelineStateDesc;
        pipelineStateDesc._identifier = "Cube";
        pipelineStateDesc._inputLayout = InputLayoutPreset::BaseInputLayout();

        // blobs
        auto VS = _shaders.find("Cube_VS");
        auto PS = _shaders.find("Cube_PS");
        pipelineStateDesc._desc.PS = {PS->second.get()->_bytecode->GetBufferPointer(),
                                      PS->second.get()->_bytecode->GetBufferSize()};
        pipelineStateDesc._desc.VS = {VS->second.get()->_bytecode->GetBufferPointer(),
                                      VS->second.get()->_bytecode->GetBufferSize()};

        pipelineStateDesc._desc.InputLayout = {pipelineStateDesc._inputLayout.GetPointer(),
                                               static_cast<UINT>(pipelineStateDesc._inputLayout.GetSize())};
        pipelineStateDesc._desc.pRootSignature = Engine::GetDX12Device()->GetRootSignature();
        pipelineStateDesc._desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pipelineStateDesc._desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pipelineStateDesc._desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        pipelineStateDesc._desc.SampleMask = UINT_MAX;
        pipelineStateDesc._desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc._desc.NumRenderTargets = 1;
        pipelineStateDesc._desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pipelineStateDesc._desc.SampleDesc.Count = 1;
        pipelineStateDesc._desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

        _pipelineStateHandler->CreatePipelineState(pipelineStateDesc);
    }
}; // namespace tb