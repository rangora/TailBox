#pragma once

namespace tb
{
    class RenderPass;

    enum RenderpassType
    {
        RENDER_PASS_FORWARD_RENDERING,
        RENDER_PASS_UI_RENDERING
    };

    class RenderPassManager
    {
    public:
        RenderPassManager();
        ~RenderPassManager();

        static void Create();
        static RenderPassManager* Get();

    private:
        static RenderPassManager* _instance;
    };
}