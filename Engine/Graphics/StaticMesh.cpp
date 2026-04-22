#include "StaticMesh.h"
#include "GraphicsCore.h"

namespace tb
{

    StaticMesh::StaticMesh(std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
    {
        _vertices = vertices;
        _indices = indices;

        g_renderAPI->SetupStaticMesh(_VOI, vertices, indices);
    }

    void StaticMesh::SetupMesh()
    {
    }
} // namespace tb