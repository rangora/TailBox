#pragma once

#include "RenderPass.h"

namespace tb
{
    class RenderPassForwardRendering : public RenderPass
    {
    public:
        RenderPassForwardRendering();
        ~RenderPassForwardRendering();

        void Render(Camera* camera) final;
    };
}