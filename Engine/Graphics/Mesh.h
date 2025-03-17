#pragma once

#include "Core.h"

namespace tb
{
    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(std::vector<Vertex>& vertexBuffer, std::vector<uint32>& indexBuffer);
        ~Mesh();

        void SetTransform(const Transform& transform) { _transform = transform; }

        void Render();

        void Clear();

        ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
        uint32 _vertexCount = 0;

        ComPtr<ID3D12Resource> _indexBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};
        uint32 _indexCount = 0;

        class GpuBuffer* _gBuffer = nullptr;

        Transform _transform = {};
    };
} // namespace tb