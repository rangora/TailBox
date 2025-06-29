#pragma once

#include "Core.h"
#include "RenderEnum.h"

namespace tb
{
    struct ShaderCompileDesc
    {
        ShaderType _shaderType = ShaderType::None;
        std::string _path;
        std::string _identifier;
    };

    class Shader
    {
    public:
        Shader();
        ~Shader();
        Shader(const std::string& vp, const std::string& pp);
        Shader(ShaderCompileDesc desc);

        ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

        ComPtr<ID3DBlob> _vsBlob = nullptr;
        ComPtr<ID3DBlob> _psBlob = nullptr;
        ComPtr<ID3DBlob> _errBlob = nullptr;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};
    };
};