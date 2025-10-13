#pragma once

#include "Core.h"
#include "RenderResource/ShaderStructure.h"

namespace tb
{
    class Mesh
    {
    public:
        uint32 _VOI = 0;

        std::vector<Vertex> _vertices;
        std::vector<uint32> _indices;

        Mesh();
        ~Mesh();

        virtual void SetupMesh() = 0;
    };
}