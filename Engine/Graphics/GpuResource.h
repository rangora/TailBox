#pragma once

#include "Core.h"

namespace tb
{
    class GpuResource
    {
    public:
        GpuResource() = default;
        GpuResource(ID3D12Resource* resource) : _resource(resource), _gpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL) {}
        virtual ~GpuResource() = default;

        ComPtr<ID3D12Resource> _resource = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS _gpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    };
}