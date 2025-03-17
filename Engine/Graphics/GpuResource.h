#pragma once

#include "Core.h"

namespace tb
{
    class GpuResource
    {
    public:
        GpuResource() = default;
        virtual ~GpuResource() = default;

        ComPtr<ID3D12Resource> _resource = nullptr;
    };
}