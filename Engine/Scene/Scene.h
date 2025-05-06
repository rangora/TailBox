#pragma once

#include "Core.h"


namespace tb
{
    class SceneProxy;

    class Scene
    {
    public:
        void Update();
        void Render();

        void AddProxy(SceneProxy* proxy);

    private:
        std::vector<SceneProxy*> _sceneProxies;
    };
}; // namespace tb