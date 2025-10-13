#pragma once

namespace tb
{
    class Camera;

    class RenderPass
    {
    public:
        RenderPass() {};
        ~RenderPass() {};

        virtual void Render(Camera* camera) = 0;
    };
}