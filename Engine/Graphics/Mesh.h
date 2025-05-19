#pragma once

#include "Core.h"
#include "UploadBuffer.h"
#include "GpuBuffer.h"
#include "core/Transform.h"

namespace tb
{
    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void SetTransform(Transform_2& transform) { _transform_old = transform; }
        void SetTransform(Transform& transform) { _transform = transform; }

        void Render();
        void Clear();

        class GpuBuffer* _gBuffer = nullptr; // CB
        GpuBuffer* _transformBuffer = nullptr;

        Transform_2 _transform_old = {};
        Transform _transform = {};
    };
} // namespace tb