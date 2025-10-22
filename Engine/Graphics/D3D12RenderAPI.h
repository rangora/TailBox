#pragma once

#include "RenderAPI.h"
#include "DirectX12/D3D12Struct.h"
#include "Utility/IndexDispenser.h"
#include <vector>

namespace tb
{
    class D3D12RenderAPI : public RenderAPI
    {
    public:
        D3D12RenderAPI();
        ~D3D12RenderAPI();

        void SetupStaticMesh(unsigned int& VOI, const std::vector<Vertex>& vertices, const std::vector<uint32>& indices) final;

        void Draw(uint32 VOI) final;

    private:
        int32 AllocateVOIndex();
        D3D12VO* GetVO(int32 idx);
        void FreeVOIndex(int32 idx);

        D3D12Buffer CreateBuffer(uint64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState,
                                 D3D12_HEAP_TYPE heapType, bool cpuAddr, bool gpuAddr, const void* data);

        std::vector<D3D12VO*> _VOs;

        ComPtr<ID3D12Device> _device = nullptr;

        IndexDispenser _idxDispenser;
    };
}