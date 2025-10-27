#pragma once

#include "RenderPass.h"

namespace tb
{
    class RenderPassUIRendering : public RenderPass
    {
    public:
        RenderPassUIRendering() = delete;
        RenderPassUIRendering(ID3D12Device* device);
        ~RenderPassUIRendering();

        void Render(const D3D12View& view) final;

    private:
        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
    };
}