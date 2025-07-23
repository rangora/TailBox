#pragma once

#include "Core.h"
#include "Actor.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"

namespace tb
{
    class CubeActor : public Actor
    {
    public:
        CubeActor(Scene* scene);
        ~CubeActor();
    };
} // namespace tb