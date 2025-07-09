#include "UploadBuffer.h"
#include "GraphicsCore.h"

namespace tb
{
    void UploadBuffer::Create(uint32 size)
    {
        _bufferSize = size;
        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

        g_dx12Device.GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                     IID_PPV_ARGS(&_resource));

        _gpuVirtualAddress = _resource->GetGPUVirtualAddress();
    }

    void* UploadBuffer::Map()
    {
        void* mem;
        CD3DX12_RANGE ranges = CD3DX12_RANGE(0, _bufferSize);
        _resource->Map(0, &ranges, &mem);
        return mem;
    }

    void UploadBuffer::Unmap(uint32 begin, uint32 end)
    {
        CD3DX12_RANGE ranges = CD3DX12_RANGE(begin, std::min(end, _bufferSize));
        _resource->Unmap(0, &ranges);
    }
} // namespace tb