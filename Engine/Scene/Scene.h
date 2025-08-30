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
        void ReleaseAllActors();

        void Update(float deltaTime);
        void Render(const XMMATRIX& vpMtx);
        void Clear();

        void RegisterActor(Actor* actor);

    private:
        std::vector<Actor*> _actors;
    };
}; // namespace tb