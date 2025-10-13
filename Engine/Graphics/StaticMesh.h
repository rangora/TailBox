#pragma once

#include "Mesh.h"

namespace tb
{
    class StaticMesh : public Mesh
    {
    public:
        StaticMesh(std::vector<Vertex>& vertices, const std::vector<uint32>& indices);

        void SetupMesh() final;
    };
}