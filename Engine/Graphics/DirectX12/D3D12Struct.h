#pragma once

#include "Core.h"

namespace tb
{
    struct D3D12Fence
    {
        uint64 _fenceValue = 0;
        ComPtr<ID3D12Fence> _fence = nullptr;
    };

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

    struct D3D12DrawCommand
    {
        uint32 _VOI = 0;
    };

    struct D3D12FrameContext
    {
        ComPtr<ID3D12CommandAllocator> _commandAllocator = nullptr;
        uint64 _fenceValue = 0;
    };

    struct D3D12View
    {
        XMMATRIX _projMtx;
        XMMATRIX _sceneViewMtx;
    };
}