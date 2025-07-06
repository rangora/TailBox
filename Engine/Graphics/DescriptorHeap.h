#pragma once

#include "Core.h"

namespace tb
{
    static constexpr uint32 CBV_REGISTER_COUNT = 14;
    static constexpr uint32 SRV_REGISTER_COUNT = 16;
    static constexpr uint32 UAV_REGISTER_COUNT = 16;

    class DescriptorHeap
    {
    public:
        DescriptorHeap(ID3D12Device* device, uint32 count);
        DescriptorHeap();
        ~DescriptorHeap();

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 reg);

        void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, D3D12_CPU_DESCRIPTOR_HANDLE destHandle);
        void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, D3D12_CPU_DESCRIPTOR_HANDLE destHandle);
        ID3D12DescriptorHeap* GetDescriptorHeap() const { return _descHeap.Get(); }

        void Clear(); // ?

        void CommitTable(const uint32 rootIndex);

    private:
        ComPtr<ID3D12DescriptorHeap> _descHeap = nullptr;
        uint64 _handleSize = 0;
        uint64 _groupSize = 0;
        uint64 _groupCount = 0;
        uint32 _currentGroupIndex = 0;
    };
}