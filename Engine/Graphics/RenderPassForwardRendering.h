#pragma once

#include "RenderPass.h"

namespace tb
{
    class RenderPassForwardRendering : public RenderPass
    {
    public:
        RenderPassForwardRendering() = delete;
        RenderPassForwardRendering(ID3D12Device* device) : RenderPass(device) {}
        ~RenderPassForwardRendering();

        void Render(const D3D12View& view) final;
    };
}