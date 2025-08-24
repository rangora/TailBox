#include "MeshRenderResource.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/GraphicsCore.h"

namespace tb
{
    MeshRenderResource::MeshRenderResource() = default;
    MeshRenderResource::~MeshRenderResource() = default;

    bool MeshRenderResource::LinkGeometryResources(const std::string& resourceKey)
    {
        _geoemtryBuffer = g_graphicsResources.GetGeometryBuffer(resourceKey);
        return _geoemtryBuffer ? true : false;
    }
} // namespace tb