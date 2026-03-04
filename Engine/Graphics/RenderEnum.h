#pragma once

namespace tb
{
    enum class ShaderType
    {
        None,
        Vertex,
        Pixel
    };

    enum RenderStageType
    {
        SCENE = 0,
        UI,
        REDNERSTAGE_MAX,
    };
};