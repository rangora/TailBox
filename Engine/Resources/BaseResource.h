#pragma once

#include <memory>
#include "Graphics/RenderResource/ShaderStructure.h"

namespace tb
{
    class GeometryBuffer;
    class TextureResource;

    namespace br
    {
        GeometryBuffer* CreateCubeBuffer();
        GeometryBuffer* CreateBoxBuffer();

        std::vector<TextureResource*> CreateDefaultTextureResource();

        static std::vector<Vertex> _cubeVertices = {
            // Front face
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // top-left
            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},  // bottom-right
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // bottom-left
            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},   // top-right

            // Right face
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, // bottom-left
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},   // top-right
            {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},  // bottom-right
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // top-left

            // Left face
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},   // top-left
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, // bottom-right
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},  // bottom-left
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},  // top-right

            // Back face
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},   // top-left
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // bottom-right
            {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // bottom-left
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // top-right

            // Top face
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // top-left
            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},   // bottom-right
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // bottom-left
            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // top-right

            // Bottom face
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // top-left
            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},  // bottom-right
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, // bottom-left
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}    // top-right
        };

        static std::vector<uint32> _cubeIndices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,
                                                   8,  9,  10, 8,  11, 9,  12, 13, 14, 12, 15, 13,
                                                   16, 18, 17, 16, 17, 19, 20, 22, 21, 20, 21, 23};
    } // namespace br
} // namespace tb