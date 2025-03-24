#pragma once

#include "GpuResource.h"

namespace tb
{
    class UploadBuffer : public GpuResource
    {
    public:
        void Create(uint32 size);
        void* Map();
        void Unmap(uint32 begin, uint32 end);

        uint32 _bufferSize = 0;
    };
};