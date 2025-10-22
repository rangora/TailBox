#pragma once

#include "RenderResource/ShaderStructure.h"

namespace tb
{
    class RenderAPI
    {
    public:
        RenderAPI();
        ~RenderAPI();

        static void Create();
        static RenderAPI* Get();

        virtual void SetupStaticMesh(unsigned int& VOI, const std::vector<Vertex>& vertices, const std::vector<uint32>& indices) = 0;

        virtual void Draw(uint32 VOI) = 0;

    private:
        static RenderAPI* _instance;
    };
} // namespace tb