#pragma once

#include "Core.h"

namespace tb
{
    class Material;

    struct DrawCall
    {
        uint32 _meshId = 0;
        XMMATRIX _worldMtx = {};
        Material* _material = nullptr;
    };
} // namespace tb
