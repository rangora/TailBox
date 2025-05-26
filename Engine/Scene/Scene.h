#pragma once

#include "Core.h"


namespace tb
{
    class SceneProxy;
    class Mesh;
    class Cube;

    class Scene
    {
    public:
        void Update();
        void Render();

        void AddProxy(SceneProxy* proxy);
        void AddMesh(Mesh* mesh);
        void AddCube(Cube* cube);

    private:
        std::vector<SceneProxy*> _sceneProxies;
        std::vector<Mesh*> _meshes;
        std::vector<Cube*> _cubes;
    };
}; // namespace tb