#pragma once

#include "Core.h"

namespace tb
{
    class IndexDispenser
    {
    public:
        IndexDispenser();
        ~IndexDispenser();

        void Initialize(uint32 size);
        void Free(uint32 index);
        void Release();

        uint32 Allocate();

    private:
        uint32* _container = nullptr;
        uint32 _maxSize = 0;
        uint32 _allocatedSize = 0;
    };
}