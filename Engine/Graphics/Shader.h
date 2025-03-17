#pragma once

#include "Core.h"

namespace tb
{
    class Shader
    {
    public:
        Shader(const std::wstring& vp, const std::wstring& pp);

        ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

    private:
        ComPtr<ID3DBlob> _vsBlob = nullptr;
        ComPtr<ID3DBlob> _psBlob = nullptr;
        ComPtr<ID3DBlob> _errBlob = nullptr;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};
    };
};