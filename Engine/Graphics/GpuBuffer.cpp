#include "GpuBuffer.h"
#include "Engine.h"

namespace tb
{
    GpuBuffer::~GpuBuffer()
    {
    }

    void GpuBuffer::CreateConstantBuffer(uint32 _size, uint32 _count)
    {
        // �켱 ��� ���۷�..
        _elementSize = (_size + 255) & ~255;
        _elementCount = _count;

        uint32 bufferSize = _elementSize * _elementCount;
        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        Engine::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                     IID_PPV_ARGS(&_resource));
        _resource->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));

        // Create view
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = _elementCount;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            Engine::GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_bufferHeap));
            _cpuHandleBegin = _bufferHeap->GetCPUDescriptorHandleForHeapStart();
            _handleIncrementSize =
                Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

            for (int i = 0; i < _elementCount; ++i)
            {
                D3D12_CPU_DESCRIPTOR_HANDLE bufferHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, i * _handleIncrementSize);
                D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc = {};
                bufferDesc.BufferLocation = _resource->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
                bufferDesc.SizeInBytes = _elementSize;
                Engine::GetDevice()->CreateConstantBufferView(&bufferDesc, bufferHandle);
            }
        }
    }
} // namespace tb