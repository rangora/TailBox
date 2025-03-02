#pragma once

#include "Core.h"
#include "imgui/imgui.h"

namespace tb
{

    struct DescHeapAllocator
    {
        ID3D12DescriptorHeap* _heap = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE _heapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        D3D12_CPU_DESCRIPTOR_HANDLE _cpuStartHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE _gpuStartHandle;
        UINT _incrementSize = 0;
        ImVector<int> _freeIndices;

        void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
        {
            IM_ASSERT(_heap == nullptr && _freeIndices.empty());
            _heap = heap;
            D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
            _heapType = desc.Type;
            _cpuStartHandle = _heap->GetCPUDescriptorHandleForHeapStart();
            _gpuStartHandle = _heap->GetGPUDescriptorHandleForHeapStart();
            _incrementSize = device->GetDescriptorHandleIncrementSize(_heapType);
            _freeIndices.reserve((int)desc.NumDescriptors);
            for (int n = desc.NumDescriptors; n > 0; n--)
                _freeIndices.push_back(n);
        }
        void Destroy()
        {
            _heap = nullptr;
            _freeIndices.clear();
        }
        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
        {
            IM_ASSERT(_freeIndices.Size > 0);
            int idx = _freeIndices.back();
            _freeIndices.pop_back();
            out_cpu_desc_handle->ptr = _cpuStartHandle.ptr + (idx * _incrementSize);
            out_gpu_desc_handle->ptr = _gpuStartHandle.ptr + (idx * _incrementSize);
        }
        void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
        {
            int cpu_idx = (int)((out_cpu_desc_handle.ptr - _cpuStartHandle.ptr) / _incrementSize);
            int gpu_idx = (int)((out_gpu_desc_handle.ptr - _gpuStartHandle.ptr) / _incrementSize);
            IM_ASSERT(cpu_idx == gpu_idx);
            _freeIndices.push_back(cpu_idx);
        }
    };
} // namespace tb