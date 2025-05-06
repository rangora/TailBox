#pragma once

#include "Core.h"

namespace tb
{
    class Scene;

    enum SceneType
    {
        World = 0,
    };

    class SceneManager
    {
    public:
        static SceneManager* Get()
        {
            static SceneManager instance;
            return &instance;
        }

        void SetLayer(uint32 index, const std::string& name);

        // Scene
        void HandleNewActor();

    private:
        SceneManager()
        {
        }
        ~SceneManager()
        {
        }

        enum
        {
            MAX_LAYER = 32
        };

        std::array<std::string, MAX_LAYER> _layers;
        std::map<std::string, uint32> _layerIndex;

        uint32 _activeIndex = -1;
    };
} // namespace tb