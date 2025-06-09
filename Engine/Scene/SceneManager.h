#pragma once

#include "Core.h"

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

        // Render
        void Render(XMMATRIX& vpMtx);
        void OnRenderBegin();

    private:
        SceneManager() = default;
        ~SceneManager() = default;

        enum
        {
            MAX_LAYER = 32
        };

        std::map<std::string, uint32> _layerIndex;
        std::array<Scene*, MAX_LAYER> _scenes;

        uint32 _activeIndex = -1;
    };
} // namespace tb