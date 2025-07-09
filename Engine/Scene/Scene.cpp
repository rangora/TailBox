#include "Scene.h"
#include "Core/Input.h"
#include "Graphics/Mesh.h"
#include "Scene/Cube.h"
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
        //_mesh = new Mesh(this);
        _cube = new Cube(this);
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

        for (const auto cube : _cubes)
        {
            cube->Render(vpMtx);
        }
    }

    void Scene::Clear()
    {
        if (_cube)
        {
            _cube->Clear();
        }
        if (_mesh)
        {
            _mesh->Clear();
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