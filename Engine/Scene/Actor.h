#pragma once

#include "Core.h"

namespace tb
{
    class SceneProxy;

    class Actor
    {
    public:
        Actor();
        ~Actor();

        void Tick(float delta);

    private:
        std::unique_ptr<SceneProxy> _sceneProxy = nullptr;
        Transform _transform;
    };

} // namespace tb