#include "DescriptorHeap.h"
#include "Engine.h"
#include "Graphics/DX12Device.h"

namespace tb
{
    DescriptorHeap::DescriptorHeap(uint32 count)
    {
        _groupCount = count;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT + UAV_REGISTER_COUNT;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

        Engine::GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));
        _handleSize = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        _groupSize = _handleSize * (SRV_REGISTER_COUNT + CBV_REGISTER_COUNT + UAV_REGISTER_COUNT - 1);

    }

    DescriptorHeap::DescriptorHeap()
    {
    }

    DescriptorHeap::~DescriptorHeap()
    {
    }

    void DescriptorHeap::Clear()
    {
        _currentGroupIndex = 0;
    }
} // namespace tb