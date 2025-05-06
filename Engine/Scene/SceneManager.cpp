#include "SceneManager.h"

namespace tb
{
    void SceneManager::SetLayer(uint32 index, const std::string& name)
    {
        const std::string preName = _layers[index];
        _layerIndex.erase(preName);

        _layers[index] = name;
        _layerIndex[name] = index;
        _activeIndex = index;
    }

    void SceneManager::HandleNewActor()
    {
    }
} // namespace tb