#pragma once

#include "Core.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"

namespace tb
{
    class GpuBuffer;
    class Scene;
    class SceneComponent;

    class Actor
    {
    public:
        Actor(Scene* scene);
        ~Actor();

        void SetLocation(const Vector& location);

        void Render(const XMMATRIX& vpMtx);
        void Clear();

        Transform GetTrnasform() const { return _transform; }

    private:
        Scene* _scene = nullptr;

        Transform _transform;
        std::vector<SceneComponent*> _registeredCompoent;
    };
} // namespace tb