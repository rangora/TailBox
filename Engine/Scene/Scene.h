#pragma once

#include "Core.h"


namespace tb
{
    class SceneProxy;
    class Mesh;

    class Scene
    {
    public:
        void Update();
        void Render();

        void AddProxy(SceneProxy* proxy);
        void AddMesh(Mesh* mesh);

    private:
        std::vector<SceneProxy*> _sceneProxies;
        std::vector<Mesh*> _meshes;
    };
}; // namespace tb