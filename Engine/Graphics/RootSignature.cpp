#include "RootSignature.h"
#include "GraphicsCore.h"

namespace tb
{
    RootSignature::RootSignature() = default;

    RootSignature::~RootSignature()
    {
        if (_rootSignature)
        {
            _rootSignature.Reset();
        }
    }

    void RootSignature::CreateMaterialRootSignature()
    {
        CD3DX12_DESCRIPTOR_RANGE srvRange = {};
        srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER params[2] = {};
        params[0].InitAsConstantBufferView(0);
        params[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);

        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                              D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                              D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                              D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_ROOT_SIGNATURE_DESC desc;
        desc.Init(_countof(params), params, 1, &sampler, flags);
        ComPtr<ID3DBlob> blobSignature;
        ComPtr<ID3DBlob> blobError;

        if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError)))
        {
            assert(false);
            return;
        }

        g_dx12Device.GetDevice()->CreateRootSignature(0, blobSignature->GetBufferPointer(),
                                                      blobSignature->GetBufferSize(),
                                                      IID_PPV_ARGS(_rootSignature.GetAddressOf()));
    }

} // namespace tb