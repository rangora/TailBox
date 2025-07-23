#pragma once

#include "SceneComponent.h"

namespace tb
{
    class Actor;
    class GpuBuffer;
    class GeometryBuffer;

    class StaticMeshComponent : public SceneComponent
    {
    public:
        StaticMeshComponent() = delete;
        StaticMeshComponent(Actor* ownerActor);
        ~StaticMeshComponent();

        void SetStaticMesh(const std::string& meshName);

        // proxy functions
        void Render(const XMMATRIX& vpMtx) final;
        void Clear() final;

    private:
        std::string _meshName;

        // proxy data
        GpuBuffer* _baseBuffer = nullptr;
        GeometryBuffer* _geoemtryBuffer = nullptr;
    };
} // namespace tb