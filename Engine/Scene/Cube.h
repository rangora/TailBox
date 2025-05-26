#pragma once

#include "Core.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"
#include "core/Transform.h"

namespace tb
{
    class GpuBuffer;

    class Cube
    {
    public:
        Cube();
        ~Cube();

        void Render();
        void Clear();

        GpuBuffer* _cBuffer = nullptr; // CB

        Matrix _matrix = {};
    };
} // namespace tb