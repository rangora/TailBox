#pragma once

#include "RenderResource/ShaderStructure.h"

namespace tb
{
    class RenderAPI
    {
    public:
        RenderAPI() = default;
        virtual ~RenderAPI() = default;

        virtual void SetupStaticMesh(unsigned int& VOI, const std::vector<Vertex>& vertices, const std::vector<uint32>& indices) = 0;
        virtual void Draw(uint32 VOI) = 0;
    };
} // namespace tb