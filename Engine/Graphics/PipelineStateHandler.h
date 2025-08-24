#pragma once

#include <vector>
#include "Core.h"
#include "InputLayout.h"

namespace tb
{
    struct GraphicsPipelineStateDesc
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC _desc = {};
        InputLayout _inputLayout;
        std::string _identifier;
        std::string _rootSigantureId;
        std::string _vsId;
        std::string _psId;
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