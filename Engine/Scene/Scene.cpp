#include "Scene.h"
#include "Graphics/Mesh.h"

namespace tb
{
    void Scene::Update()
    {
    }

    void Scene::Render()
    {
        for (const auto mesh : _meshes)
        {
            mesh->Render();
        }
    }

    void Scene::AddProxy(SceneProxy* proxy)
    {
    }

    void Scene::AddMesh(Mesh* mesh)
    {
        _meshes.emplace_back(mesh);
    }
} // namespace tb