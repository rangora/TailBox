#include "Renderer.h"
#include "InputLayout.h"
#include "PipelineStateHandler.h"
#include "Resources/BaseResource.h"
#include "ShaderCompiler.h"
#include "TextureResource.h"
#include "UploadBuffer.h"
#include "RootSignature.h"

namespace tb
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::InitRootSignature()
    {
        // Base
        std::unique_ptr<RootSignature> basePS = std::make_unique<RootSignature>();
        basePS->CreateRootSignature();
        _rootSignatures.emplace("Default", std::move(basePS));

        // Material
        std::unique_ptr<RootSignature> materialRS = std::make_unique<RootSignature>();
        materialRS->CreateMaterialRootSignature();
        _rootSignatures.emplace("Material", std::move(materialRS));
    }

    void Renderer::Initialize()
    {
        _pipelineStateHandler = std::make_unique<PipelineStateHandler>();

        _geoemtryBuffers.reserve(100);
        _shaders.reserve(100);
        _rootSignatures.reserve(100);

        InitRootSignature();
        InitBuffers();
        InitShaders();

        // Init textures
        std::unique_ptr<TextureResource> texture = std::make_unique<TextureResource>();
        texture->CreateTexture(tb::core::projectPath + "/Resources/Texture/niko.png");
        _textures.emplace("Niko", std::move(texture));
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
        shaderCompileDescs.push_back({ShaderType::Vertex, tb::core::projectPath + "/Resources/Shader/MaterialVS.hlsl", "Material_VS"});
        shaderCompileDescs.push_back({ShaderType::Pixel, tb::core::projectPath + "/Resources/Shader/MaterialPS.hlsl", "Material_PS"});

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
        pipelineStateDesc._desc.pRootSignature = _rootSignatures.find("Default")->second->_rootSignature.Get();
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

        CreateMaterialPipelineState();
    }

    void Renderer::CreateMaterialPipelineState()
    {
        GraphicsPipelineStateDesc pipelineStateDesc;
        pipelineStateDesc._identifier = "Material";
        pipelineStateDesc._inputLayout = InputLayoutPreset::MaterialInputLayout();

        auto VS = _shaders.find("Material_VS");
        auto PS = _shaders.find("Material_PS");

        pipelineStateDesc._desc.PS = {PS->second.get()->_bytecode->GetBufferPointer(),
                                      PS->second.get()->_bytecode->GetBufferSize()};
        pipelineStateDesc._desc.VS = {VS->second.get()->_bytecode->GetBufferPointer(),
                                      VS->second.get()->_bytecode->GetBufferSize()};

        // Inputlayout..
        pipelineStateDesc._desc.InputLayout = {pipelineStateDesc._inputLayout.GetPointer(),
                                               static_cast<UINT>(pipelineStateDesc._inputLayout.GetSize())};
        pipelineStateDesc._desc.pRootSignature = _rootSignatures.find("Material")->second->_rootSignature.Get();
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