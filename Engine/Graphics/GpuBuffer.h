#pragma once

#include "GpuResource.h"

namespace tb
{
    class GpuBuffer : public GpuResource
    {
    public:
        GpuBuffer() = default;
        ~GpuBuffer();

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
} // namespace tb