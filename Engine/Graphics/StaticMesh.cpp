#include "StaticMesh.h"
#include "RenderAPI.h"

namespace tb
{

    StaticMesh::StaticMesh(std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
    {
        _vertices = vertices;
        _indices = indices;

        RenderAPI::Get()->SetupStaticMesh(_VOI, vertices, indices);
    }

    void StaticMesh::SetupMesh()
    {
    }
} // namespace tb