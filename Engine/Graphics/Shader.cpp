#include "Shader.h"
#include "Engine.h"
#include "DX12Device.h"

namespace tb
{
    Shader::Shader()
    {
    }

    Shader::~Shader()
    {
    }

    Shader::Shader(const std::wstring& vp, const std::wstring& pp)
    {
#ifdef _DEBUG
            uint32 compileFlag = 0;
            compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
         // Vertex shader
        {
            if (FAILED(::D3DCompileFromFile(vp.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Main", "vs_5_0",
                                            compileFlag, 0, &_vsBlob, &_errBlob)))
            {
                // failed..
            }

            _pipelineDesc.VS = {_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize()};
        }

        // Pixel shader
        {
            if (FAILED(::D3DCompileFromFile(vp.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Main", "ps_5_0",
                                            compileFlag, 0, &_psBlob, &_errBlob)))
            {
                // failed..
            }

            _pipelineDesc.PS = {_psBlob->GetBufferPointer(), _psBlob->GetBufferSize()};
        }

        D3D12_INPUT_ELEMENT_DESC desc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

        _pipelineDesc.InputLayout = {desc, _countof(desc)};
        _pipelineDesc.pRootSignature = Engine::GetDX12Device()->GetRootSignature();
        _pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        _pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        _pipelineDesc.DepthStencilState.DepthEnable = FALSE;
        _pipelineDesc.DepthStencilState.StencilEnable = FALSE;
        _pipelineDesc.SampleMask = UINT_MAX;
        _pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        _pipelineDesc.NumRenderTargets = 1;
        _pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        _pipelineDesc.SampleDesc.Count = 1;

        Engine::GetDevice()->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
    }
} // namespace tb
