#pragma once

#include "Core.h"
#include "Camera.h"

namespace tb
{
    class Scene;
    class Mesh;
    class Actor;

    enum SceneType
    {
        World = 0,
    };

    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager() = default;

        void SetLayer(uint32 index, const std::string& name);

        void Update(float deltaTime);
        void OnEndFrame();

        // Render
        void Render();
        void OnRenderBegin();

    private:
        enum
        {
            MAX_LAYER = 32
        };

        std::map<std::string, uint32> _layerIndex;
        std::array<Scene*, MAX_LAYER> _scenes;

        uint32 _activeIndex = -1;

        Camera _camera;
    };
} // namespace tb