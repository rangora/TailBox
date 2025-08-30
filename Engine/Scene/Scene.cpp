#include "Scene.h"
#include "Core/Input.h"
#include "Scene/CubeActor.h"
#include "Scene/StaticMeshComponent.h"
#include "Window/Window.h"
#include "Graphics/Material.h"

namespace tb
{
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
        for (Actor* actor : _actors)
        {
            delete actor;
        }
    }

    void Scene::Initialize()
    {
        // spawnActor로 wrapping 필
        // 호출 위치도 바뀌어야 한다.
        auto Cube1 = new CubeActor(this);
        Cube1->SetLocation({0.f, 0.f, 0.f});
        Material* material1 = Cube1->_staticMeshComponent->GetMaterial();
        material1->BindTextures(TextureType::BASECOLOR, "light");

        auto Cube2 = new CubeActor(this);
        Cube2->SetLocation({3.f, 0.f, 0.f});
        Cube2->SetScale({0.3f, 0.3f, 0.3f});
        Material* material2 = Cube2->_staticMeshComponent->GetMaterial();
        material2->BindTextures(TextureType::BASECOLOR, "light");
    }

    void Scene::ReleaseAllActors()
    {
        for (Actor* actor : _actors)
        {
            delete actor;
        }
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