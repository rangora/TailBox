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
    }

    void SceneManager::RegisterMesh(Mesh* mesh)
    {
        auto scene = _scenes[_activeIndex];
        scene->AddMesh(mesh);
    }

    void SceneManager::Render()
    {
        Scene* scene = _scenes[_activeIndex];
        scene->Render();
    }
} // namespace tb