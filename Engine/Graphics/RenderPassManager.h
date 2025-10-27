#pragma once

#include <vector>
#include "DirectX12/D3D12Struct.h"

namespace tb
{
    class RenderPass;

    enum RenderpassType
    {
        RENDER_PASS_FORWARD_RENDERING = 0,
        RENDER_PASS_UI_RENDERING,
        MAX
    };

    enum RenderPiplineType
    {
        Forward,
    };

    class RenderPassManager
    {
    public:
        RenderPassManager();
        ~RenderPassManager();

        static void Create();
        static RenderPassManager* Get();

        void SetupRenderPass(RenderPiplineType type);
        void Render(const D3D12View& view);

    private:
        std::vector<RenderPass*> _currentPasses;
        std::vector<RenderPass*> _renderPasses;

        static RenderPassManager* _instance;
    };
}