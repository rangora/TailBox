#pragma once

#include "Core.h"
#include "string"

namespace tb
{
    struct MeshResource
    {
        ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

        ComPtr<ID3D12Resource> _indexBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};
        uint32 _indexCount = 0;
    };

    class GpuResource
    {
    public:
        GpuResource() = default;
        GpuResource(ID3D12Resource* resource) : _resource(resource), _gpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL) {}
        virtual ~GpuResource() { Destroy(); }

        virtual void Destroy()
        {
            if (_resource)
            {
                _resource.Reset();
            }
            _gpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
        }

        ComPtr<ID3D12Resource> _resource = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS _gpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
        std::string _alias;
    };
}