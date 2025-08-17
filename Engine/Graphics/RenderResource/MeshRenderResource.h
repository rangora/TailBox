#pragma once

#include "Core.h"

namespace tb
{
    class GeometryBuffer;

    class MeshRenderResource
    {
    public:
        MeshRenderResource();
        ~MeshRenderResource();

        bool LinkGeometryResources(const std::string& resourceKey);
        const GeometryBuffer* GetGeometryBuffer() const { return _geoemtryBuffer; }

    protected:
        GeometryBuffer* _geoemtryBuffer = nullptr;
    };
} // namespace tb