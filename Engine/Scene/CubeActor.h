#pragma once

#include "Core.h"
#include "Actor.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"

namespace tb
{
    class StaticMeshComponent;

    class CubeActor : public Actor
    {
    public:
        CubeActor(Scene* scene);
        virtual ~CubeActor();

        StaticMeshComponent* _staticMeshComponent = nullptr;
    };
} // namespace tb