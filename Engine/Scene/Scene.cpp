#include "Scene.h"
#include "Graphics/Mesh.h"
#include "Scene/Cube.h"

namespace tb
{
    void Scene::Update()
    {
    }

    void Scene::Render()
    {
        for (const auto mesh : _meshes)
        {
            //mesh->Render();
        }

        for (const auto cube : _cubes)
        {
            cube->Render();
        }
    }

    void Scene::AddProxy(SceneProxy* proxy)
    {
    }

    void Scene::AddMesh(Mesh* mesh)
    {
        _meshes.emplace_back(mesh);
    }

    void Scene::AddCube(Cube* cube)
    {
        _cubes.emplace_back(cube);
    }
} // namespace tb