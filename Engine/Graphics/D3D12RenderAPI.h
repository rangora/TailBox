#pragma once

#include "RenderAPI.h"
#include "DirectX12/D3D12Struct.h"
#include <vector>

namespace tb
{
    class D3D12RenderAPI : public RenderAPI
    {
    public:
        D3D12RenderAPI();
        ~D3D12RenderAPI();

        void SetupStaticMesh(unsigned int VOI, const std::vector<Vertex>& vertices, const std::vector<uint32>& indices) final;

    private:
        std::vector<D3D12VO> _VOs;
    };
}