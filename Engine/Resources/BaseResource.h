#pragma once

#include <memory>
#include "Graphics/PipelineStateHandler.h"

namespace tb
{
    class GeometryBuffer;
    class TextureResource;
    class GraphicsPipelineStateDesc;

    namespace br
    {
        GeometryBuffer* CreateCubeBuffer();
        GeometryBuffer* CreateBoxBuffer();

        std::vector<TextureResource*> CreateDefaultTextureResource();
        std::vector<GraphicsPipelineStateDesc> CreateDefaultPipelineState();
    }
} // namespace tb