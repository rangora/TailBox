#include "PipelineStateHandler.h"
#include "GraphicsCore.h"
#include "spdlog/spdlog.h"

namespace tb
{
    PipelineStateHandler::PipelineStateHandler()
    {
    }

    PipelineStateHandler::~PipelineStateHandler()
    {
        for (auto& [_, resource] : _pipelineStates)
        {
            resource.Reset();
        }
    }

    bool PipelineStateHandler::CreatePipelineState(GraphicsPipelineStateDesc& desc)
    {
        if (_pipelineStates.find(desc._identifier) != _pipelineStates.end())
        {
            spdlog::warn("Already has pipelineState for this ShaderName:{}"), desc._identifier;
            return false;
        }

        ComPtr<ID3D12PipelineState> pipelineState;

        if (FAILED(g_dx12Device.GetDevice()->CreateGraphicsPipelineState(&desc._desc, IID_PPV_ARGS(&pipelineState))))
        {
            spdlog::error("Failed to create graphics pipeline state");
            return false;
        }

        _pipelineStates[desc._identifier] = pipelineState;
        return true;
    }

    ComPtr<ID3D12PipelineState> PipelineStateHandler::GetPipelineState(const std::string& name)
    {
        auto it = _pipelineStates.find(name);
        return it == _pipelineStates.end() ? nullptr : it->second;
    }

    void PipelineStateHandler::Release()
    {
        for (auto& [_, state] : _pipelineStates)
        {
            if (state)
            {
                state.Reset();
            }
        }
        _pipelineStates.clear();
    }

} // namespace tb