#include "DescriptorHeap.h"
#include "GraphicsCore.h"

namespace tb
{
    DescriptorHeap::DescriptorHeap(ID3D12Device* device, uint32 count)
    {
        _groupCount = count;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT + UAV_REGISTER_COUNT;
        //desc.NumDescriptors = count * 5; // origin
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

        device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_descHeap.GetAddressOf()));
        _handleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        _groupSize = _handleSize * (SRV_REGISTER_COUNT + CBV_REGISTER_COUNT + UAV_REGISTER_COUNT - 1);
        //_groupSize = _handleSize * 5; // origin

    }

    DescriptorHeap::DescriptorHeap()
    {
    }

    DescriptorHeap::~DescriptorHeap()
    {
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(uint32 reg)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += _currentGroupIndex * _groupSize;
        handle.ptr += reg * _handleSize;
        return handle;
    }

    void DescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, D3D12_CPU_DESCRIPTOR_HANDLE destHandle)
    {
        uint32 destRange = 1;
        uint32 srcRange = 1;
        g_dx12Device.GetDevice()->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange,
                                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void DescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, D3D12_CPU_DESCRIPTOR_HANDLE destHandle)
    {
        uint32 destRange = 1;
        uint32 srcRange = 1;
        g_dx12Device.GetDevice()->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange,
                                             D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void DescriptorHeap::Clear()
    {
        _currentGroupIndex = 0;
    }

    void DescriptorHeap::CommitTable(const uint32 rootIndex)
{
        D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = _descHeap->GetGPUDescriptorHandleForHeapStart();
        GPUHandle.ptr += _currentGroupIndex * _groupSize;

        g_dx12Device.GetCommmandList()->SetGraphicsRootDescriptorTable(rootIndex, GPUHandle);
        _currentGroupIndex++;
    }
} // namespace tb