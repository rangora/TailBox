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

        void SetTransform(Transform_2& transform) { _transform = transform; }

        void Render();
        void Clear();

        class GpuBuffer* _gBuffer = nullptr; // CB

        Transform_2 _transform = {};
    };
} // namespace tb