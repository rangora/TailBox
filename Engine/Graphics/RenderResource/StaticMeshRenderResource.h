#pragma once

#include "Core.h"
#include "MeshRenderResource.h"

namespace tb
{
    class Material;

    struct StaticMeshResourceInitialData
    {
        std::string _meshName;
    };

    class StaticMeshRenderResource : public MeshRenderResource
    {
    public:
        StaticMeshRenderResource();
        virtual ~StaticMeshRenderResource();

        void LinkResources(const StaticMeshResourceInitialData& data);

        Material* GetMaterial() const { return _material; }

    private:
        Material* _material = nullptr;
    };
} // namespace tb