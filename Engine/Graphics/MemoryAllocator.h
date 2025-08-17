#pragma once

#include "Core.h"

namespace tb
{
    class DescriptorPool
    {
    public:
        DescriptorPool();
        ~DescriptorPool();

        void Initialize(int32 maxDescriptorCount);
        void Reset();

        bool AllocDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle,
                             int32 descriptorCount);

        ID3D12DescriptorHeap* GetDescriptorHeap() const { return _descriptorHeap.Get(); }

    private:
        void Clean();

        ComPtr<ID3D12DescriptorHeap> _descriptorHeap = nullptr;

        D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = {};

        int32 _allocatedDescriptorCount = 0;
        int32 _maxDescriptorCount = 0;
        int32 _resourceDescriptorSize = 0;
    };

    struct CBBlock
    {
        D3D12_CPU_DESCRIPTOR_HANDLE _handle;
        D3D12_GPU_VIRTUAL_ADDRESS _gpuMemAddr = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
        UINT8* _cpuMemAddr = nullptr;
    };

    class ConstantBufferPool
    {

    public:
        ConstantBufferPool();
        ~ConstantBufferPool();

        void Initialize(int32 blockSize, int32 maxSize);
        void Reset();

        CBBlock* Allocate();

        ComPtr<ID3D12DescriptorHeap> _descriptorHeap = nullptr;
    private:
        void Clean();

        ComPtr<ID3D12Resource> _resource = nullptr;
        UINT8* _cpuMemAddr = nullptr;
        int32 _blockSize = 0;
        int32 _maxSize = 0;
        int32 _allocatedBlocks = 0;

        CBBlock* _cbBlocks = nullptr;

    };
}