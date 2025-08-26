#include "StaticMeshRenderResource.h"
#include "Graphics/Material.h"

namespace tb
{
    StaticMeshRenderResource::StaticMeshRenderResource()
    {
        _material = new Material;
        _material->InitializeDefaultProperties();
    }

    StaticMeshRenderResource::~StaticMeshRenderResource()
    {
        if (_material)
        {
            delete _material;
        }
    }

    void StaticMeshRenderResource::LinkResources(const StaticMeshResourceInitialData& data)
    {
        LinkGeometryResources(data._meshName);

        _material = new Material;
        _material->InitializeDefaultProperties();
    }
} // namespace tb