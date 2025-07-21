#pragma once

#include "Core.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"

namespace tb
{
    class GpuBuffer;
    class Scene;

    class Actor
    {
    public:
        Actor(Scene* scene);
        ~Actor();

        void SetLocation(const Vector& location);

        void Render(const XMMATRIX& vpMtx);
        void Clear();

    private:
        Transform _transform;
        Matrix _matrix = {};

        Scene* _scene = nullptr;

        // sceneProxy
        GpuBuffer* _cBuffer = nullptr; // CB
    };
} // namespace tb