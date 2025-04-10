#include "GpuBuffer.h"
#include "Engine.h"
#include "DX12Device.h"
#include "UploadBuffer.h"

namespace tb
{
    GpuBuffer::~GpuBuffer()
    {
    }

    void GpuBuffer::CreateConstantBuffer(uint32 _size, uint32 _count)
    {
        _elementSize = (_size + 255) & ~255;
        _elementCount = _count;

        uint32 bufferSize = _elementSize * _elementCount;
        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        Engine::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                     IID_PPV_ARGS(&_resource));
        _resource->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
        _gpuVirtualAddress = _resource->GetGPUVirtualAddress();

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
                bufferDesc.BufferLocation = _gpuVirtualAddress + static_cast<uint64>(_elementSize) * i;
                bufferDesc.SizeInBytes = _elementSize;
                Engine::GetDevice()->CreateConstantBufferView(&bufferDesc, bufferHandle);
            }
        }

    }

    void GeometryBuffer::Create(const std::wstring& name, int32 size, class UploadBuffer* buffer, std::vector<Vertex>& vertexArray)
    {
        _bufferSize = size;
        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

        Engine::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                     D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                     IID_PPV_ARGS(&_resource));

        _gpuVirtualAddress = _resource->GetGPUVirtualAddress();
        _resource->SetName(name.c_str());



        {

            D3D12_SUBRESOURCE_DATA subData = {};
            subData.pData = reinterpret_cast<BYTE*>(vertexArray.data());
            subData.RowPitch = size;
            subData.SlicePitch = size;

            Engine::GetDX12Device()->Flush();
            UpdateSubresources(Engine::GetDX12Device()->GetCommmandList(), _resource.Get(), buffer->_resource.Get(), 0,
                               0, 1, &subData);

            CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

            Engine::GetDX12Device()->GetCommmandList()->ResourceBarrier(1, &resourceBarrier);

            Engine::GetDX12Device()->GetCommmandList()->Close();
            ID3D12CommandList* ppCommandLists[] = {Engine::GetDX12Device()->GetCommmandList()};
            Engine::GetDX12Device()->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);

            // staged
            Engine::GetDX12Device()->StageBuffer(buffer);
        }
    }
} // namespace tb