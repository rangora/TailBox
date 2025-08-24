#pragma once

#include <memory>
#include "Graphics/PipelineStateHandler.h"

namespace tb
{
    class GeometryBuffer;
    class TextureResource;

    namespace br
    {
        GeometryBuffer* CreateCubeBuffer();
        GeometryBuffer* CreateBoxBuffer();

        std::vector<TextureResource*> CreateDefaultTextureResource();
    }
} // namespace tb