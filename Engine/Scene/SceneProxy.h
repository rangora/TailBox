#pragma once

#include "Core.h"

namespace tb
{
    class SceneProxy
    {
    public:
        SceneProxy();
        ~SceneProxy();

        virtual void Render();
        virtual void Tick(float delta);

    private:
        Transform _transform;

        bool _bRenderMarkDirty = false;
    };
} // namespace tb