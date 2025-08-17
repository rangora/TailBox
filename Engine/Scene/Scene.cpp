#include "Scene.h"
#include "Core/Input.h"
#include "Scene/CubeActor.h"
#include "Window/Window.h"

namespace tb
{
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::Initialize()
    {
        // spawnActor로 wrapping 필
        // 호출 위치도 바뀌어야 한다.
        auto Cube1 = new CubeActor(this);
        Cube1->SetLocation({0.f, 0.f, 0.f});

        auto Cube2 = new CubeActor(this);
        Cube2->SetLocation({3.f, 0.f, 0.f});
    }

    void Scene::Update(float deltaTime)
    {
    }

    void Scene::Render(const XMMATRIX& vpMtx)
{
        for (const auto cube : _actors)
        {
            cube->Render(vpMtx);
        }
    }

    void Scene::Clear()
    {
        for (auto it : _actors)
        {
            it->Clear();
        }
    }

    void Scene::RegisterActor(Actor* actor)
    {
        _actors.emplace_back(actor);
    }
} // namespace tb