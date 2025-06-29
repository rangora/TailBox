#pragma once

#include <vector>
#include "Core.h"

namespace tb
{
    struct GraphicsPipelineStateDesc
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC _desc = {};
        std::vector<D3D12_INPUT_ELEMENT_DESC> _inputElements;
        std::string _identifier;

        ComPtr<ID3DBlob> _vsBlob = nullptr;
        ComPtr<ID3DBlob> _psBlob = nullptr;
    };

    class PipelineStateHandler
    {
    public:
        PipelineStateHandler();
        PipelineStateHandler(PipelineStateHandler&& rhs) = delete;
        PipelineStateHandler(const PipelineStateHandler& rhs) = delete;
        PipelineStateHandler& operator=(const PipelineStateHandler& rhs) = delete;
        PipelineStateHandler& operator=(PipelineStateHandler&& rhs) = delete;
        ~PipelineStateHandler();

        bool CreatePipelineState(GraphicsPipelineStateDesc& desc);
        ComPtr<ID3D12PipelineState> GetPipelineState(const std::string& name);

    private:
        std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> _pipelineStates;
    };

};