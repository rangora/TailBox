#pragma once

#include "SceneComponent.h"
#include "Graphics/RenderResource/StaticMeshRenderResource.h"

namespace tb
{
    class Actor;
    class GeometryBuffer;
    class Material;

    class StaticMeshComponent : public SceneComponent
    {
    public:
        StaticMeshComponent() = delete;
        StaticMeshComponent(Actor* ownerActor);
        ~StaticMeshComponent();

        virtual bool CheckResourceValidation() const final;

        void SetStaticMesh(const std::string& meshName);

        // proxy functions
        void Render(const XMMATRIX& vpMtx) final;
        void Clear() final;

    private:
        std::string _meshName;

        StaticMeshRenderResource _renderResource;
    };
} // namespace tb