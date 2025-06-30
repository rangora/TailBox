#pragma once

#include <memory>

namespace tb
{
    class GeometryBuffer;

    namespace br
    {
        std::unique_ptr<GeometryBuffer> CreateCubeBuffer();
        std::unique_ptr<GeometryBuffer> CreateBoxBuffer();
    }
} // namespace tb