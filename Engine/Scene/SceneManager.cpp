#include "SceneManager.h"
#include "Scene.h"
#include "Graphics/Mesh.h"

namespace tb
{
    void SceneManager::SetLayer(uint32 index, const std::string& name)
    {
        if (_layerIndex.find(name) == _layerIndex.end())
        {
            _activeIndex = _scenes.size();
            _layerIndex.insert({name, _activeIndex});
            _scenes[index] = new Scene;
        }

        _activeIndex = index;

        Scene* newScene = _scenes[_activeIndex];
        newScene->Initialize();
    }

    void SceneManager::RegisterMesh(Mesh* mesh)
    {
        auto scene = _scenes[_activeIndex];
        scene->AddMesh(mesh);
    }

    void SceneManager::RegisterMesh(Cube* cube)
    {
        auto scene = _scenes[_activeIndex];
        scene->AddCube(cube);
    }

    void SceneManager::Render(XMMATRIX& vpMtx)
    {
        Scene* scene = _scenes[_activeIndex];
        scene->Render(vpMtx);
    }

    void SceneManager::OnRenderBegin()
    {
        _scenes[_activeIndex]->Clear();
    }
} // namespace tb