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

        void RegisterActor(Actor* actor);

    private:
        std::vector<SceneProxy*> _sceneProxies; // proxy는 다음에..
        std::vector<Actor*> _actors;
    };
}; // namespace tb