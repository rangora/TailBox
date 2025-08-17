#pragma once

#include "GpuResource.h"

namespace tb
{
    class GpuBuffer : public GpuResource
    {
    public:
        GpuBuffer() = default;
        virtual ~GpuBuffer();

        void CreateConstantBuffer(uint32 _size, uint32 _count);

        BYTE* _mappedBuffer = nullptr;
        uint32 _bufferSize = 0; // ?
        uint32 _elementCount = 0;
        uint32 _elementSize = 0;
        uint32 _currentIdx = 0;

        // 여기가 적절한지 판단 필요
        ComPtr<ID3D12DescriptorHeap> _bufferHeap = nullptr;

        // 필요한지 판단 필요
        int _reg = 0; // temp
        D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandleBegin = {};
        uint32 _handleIncrementSize = 0;
    };

    class GeometryBuffer : public GpuBuffer
    {
    public:
        GeometryBuffer() = default;
        virtual ~GeometryBuffer();

        void Create(const std::wstring& name, int32 size, class UploadBuffer* buffer, std::vector<Vertex>& vertexArray);

        ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

        ComPtr<ID3D12Resource> _indexBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};
        uint32 _indexCount = 0;
    };
} // namespace tb