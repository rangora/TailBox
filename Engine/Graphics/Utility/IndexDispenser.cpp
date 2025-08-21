#include "IndexDispenser.h"

namespace tb
{
    IndexDispenser::IndexDispenser() = default;
    IndexDispenser::~IndexDispenser()
    {
        Release();
    }

    void IndexDispenser::Initialize(uint32 size)
    {
        _container = new uint32[size];
        memset(_container, 0, sizeof(uint32) * size);
        _maxSize = size;

        for (uint32 i = 0; i < _maxSize; ++i)
        {
            _container[i] = i;
        }
    }

    void IndexDispenser::Free(uint32 index)
    {
        if (_allocatedSize <= 0)
        {
            assert(false);
            return;
        }

        _allocatedSize--;
        _container[_allocatedSize] = index;
    }

    void IndexDispenser::Release()
    {
        if (_container)
        {
            delete[] _container;
            _container = nullptr;
        }
    }

    uint32 IndexDispenser::Allocate()
    {
        uint32 ret = -1;
        if (_allocatedSize >= _maxSize)
        {
            return ret;
        }

        ret = _container[_allocatedSize++];
        return ret;
    }
} // namespace tb