#include "MemoryAllocator.h"
#include "Graphics/GraphicsCore.h"

namespace tb
{
    DescriptorPool::DescriptorPool() = default;

    DescriptorPool::~DescriptorPool()
    {
        Clean();
    }

    void DescriptorPool::Initialize(int32 maxDescriptorCount)
    {
        _maxDescriptorCount = maxDescriptorCount;
        _resourceDescriptorSize = g_dx12Device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = _maxDescriptorCount;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (S_OK != g_dx12Device.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descriptorHeap)))
        {
            spdlog::error("Failed to create descriptorHeap for pool.");
            return;
        }

        _cpuHandle = _descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        _gpuHandle = _descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    void DescriptorPool::Reset()
    {
        _allocatedDescriptorCount = 0;
    }

    bool DescriptorPool::AllocDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle,
                                         D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle, int32 descriptorCount)
    {
        if (_allocatedDescriptorCount + descriptorCount > _maxDescriptorCount)
        {
            spdlog::error("Failed allocate descriptor from pool cur:{}, req:{}", _allocatedDescriptorCount, descriptorCount);
            return false;
        }

        int32 offset = _allocatedDescriptorCount + descriptorCount;
        *outCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandle, _allocatedDescriptorCount, _resourceDescriptorSize);
        *outGpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuHandle, _allocatedDescriptorCount, _resourceDescriptorSize);
        _allocatedDescriptorCount += descriptorCount;

        return true;
    }

    void DescriptorPool::Clean()
    {
        if (_descriptorHeap)
        {
            _descriptorHeap.Reset();
        }
    }

    ConstantBufferPool::ConstantBufferPool() = default;
    ConstantBufferPool::~ConstantBufferPool() = default;

    void ConstantBufferPool::Initialize(int32 blockSize, int32 maxSize)
    {
        _maxSize = maxSize;
        _blockSize = blockSize;
        int32 totalBytes = _blockSize * _maxSize;

        D3D12_HEAP_PROPERTIES bufferProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);
        if (S_OK != g_dx12Device.GetDevice()->CreateCommittedResource(&bufferProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                                       IID_PPV_ARGS(&_resource)))
        {
            spdlog::error("Failed to create uploadheap for constantBuffer pool.");
            return;
        }

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = _maxSize;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (S_OK != g_dx12Device.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_descriptorHeap)))
        {
            spdlog::error("Failed to create descriptorHeap for constantBuffer pool.");
            return;
        }

        _cbBlocks = new CBBlock[_maxSize];

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = _resource->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = _blockSize;
        int32* cpuMemPointer = _cpuMemAddr;
        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

        int32 descriptorSize =
            g_dx12Device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        for (int i = 0; i < _maxSize; ++i)
        {
            g_dx12Device.GetDevice()->CreateConstantBufferView(&cbvDesc, handle);
            _cbBlocks[i]._handle = handle;
            _cbBlocks[i]._gpuMemAddr = cbvDesc.BufferLocation;
            _cbBlocks[i]._cpuMemAddr = cpuMemPointer;

            handle.Offset(1, descriptorSize);
            cbvDesc.BufferLocation += _blockSize;
            cpuMemPointer += _blockSize;
        }
    }

    void ConstantBufferPool::Clean()
    {
        if (_cbBlocks)
        {
            delete[] _cbBlocks;
        }

        if (_resource)
        {
            _resource.Reset();
        }
        if (_descriptorHeap)
        {
            _descriptorHeap.Reset();
        }
    }

    CBBlock* ConstantBufferPool::Allocate()
    {
        if (_allocatedBlocks >= _maxSize)
        {
            return nullptr;
        }

        CBBlock* block = _cbBlocks + _allocatedBlocks++;
        return block;
    }

    void ConstantBufferPool::Reset()
    {
        _allocatedBlocks = 0;
    }
} // namespace tb