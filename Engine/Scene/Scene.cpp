#include "Scene.h"
#include "Core/Input.h"
#include "Graphics/Mesh.h"
#include "Scene/Actor.h"
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
        new Actor(this);
    }

    void Scene::Update(float deltaTime)
    {
    }

    void Scene::Render(const XMMATRIX& vpMtx)
{
        for (const auto mesh : _meshes)
        {
            mesh->Render(vpMtx);
        }

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

    void Scene::AddMesh(Mesh* mesh)
    {
        _meshes.emplace_back(mesh);
    }

    void Scene::RegisterActor(Actor* actor)
    {
        _actors.emplace_back(actor);
    }
} // namespace tb