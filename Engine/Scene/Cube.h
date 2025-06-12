#pragma once

#include "Core.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"
#include "core/Transform.h"

namespace tb
{
    class GpuBuffer;
    class Scene;

    class Cube
    {
    public:
        Cube(Scene* scene);
        ~Cube();

        void Render(const XMMATRIX& vpMtx);
        void Clear();

        GpuBuffer* _cBuffer = nullptr; // CB

        Matrix _matrix = {};

    private:
        Scene* _scene = nullptr;
    };
} // namespace tb