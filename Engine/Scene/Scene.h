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
        Scene();
        ~Scene();

        void Initialize();

        void Update();
        void Render(const XMMATRIX& vpMtx);
        void Clear();

        void AddProxy(SceneProxy* proxy);
        void AddMesh(Mesh* mesh);
        void AddCube(Cube* cube);

    private:
        std::vector<SceneProxy*> _sceneProxies;
        std::vector<Mesh*> _meshes;
        std::vector<Cube*> _cubes;

        Mesh* _mesh = nullptr;
        Cube* _cube = nullptr;
    };
}; // namespace tb