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

        // ���Ⱑ �������� �Ǵ� �ʿ�
        ComPtr<ID3D12DescriptorHeap> _bufferHeap = nullptr;

        // �ʿ����� �Ǵ� �ʿ�
        int _reg = 0; // temp
        D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandleBegin = {};
        uint32 _handleIncrementSize = 0;
    };

    class GeometryBuffer : public GpuBuffer
    {
    public:
        GeometryBuffer() = default;
        virtual ~GeometryBuffer() = default;

        void Create(const std::wstring& name, int32 size, class UploadBuffer* buffer, std::vector<Vertex>& vertexArray);
    };
} // namespace tb