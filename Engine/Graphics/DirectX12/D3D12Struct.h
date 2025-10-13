#pragma once

#include "Core.h"

namespace tb
{
    struct D3D12Buffer
    {
        ComPtr<ID3D12Resource> _buffer = nullptr;
        uint32 _size = 0;
        void* _cpuAddr = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS _gpuAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    };

    struct D3D12VO
    {
        uint32 _indexCount = 0;
        D3D12Buffer _indexBuffer;
        D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};

        uint32 _vertexCount = 0;
        D3D12Buffer _vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
    };
}