#pragma once

#include "Core.h"

namespace tb
{
    class SceneProxy;
    class Mesh;
    class Actor;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void Initialize();

        void Update(float deltaTime);
        void Render(const XMMATRIX& vpMtx);
        void Clear();

        void AddMesh(Mesh* mesh); // will be deprecated
        void RegisterActor(Actor* actor);

    private:
        std::vector<SceneProxy*> _sceneProxies; // proxy는 다음에..
        std::vector<Mesh*> _meshes; // will be deprecated
        std::vector<Actor*> _actors;
    };
}; // namespace tb