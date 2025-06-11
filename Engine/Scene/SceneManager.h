#pragma once

#include "Core.h"
#include "Camera.h"

namespace tb
{
    class Scene;
    class Mesh;
    class Cube;

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
        void RegisterMesh(Mesh* mesh);
        void RegisterMesh(Cube* cube);

        void Update(float deltaTime);
        void OnEndFrame();

        // Render
        void Render();
        void OnRenderBegin();

    private:
        SceneManager();
        ~SceneManager() = default;

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