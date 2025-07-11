#include "RootSignature.h"
#include "GraphicsCore.h"

namespace tb
{
    void RootSignature::CreateRootSignature()
    {
        CD3DX12_DESCRIPTOR_RANGE ranges[] = {CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 5, 0),
                                             CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0)};
        CD3DX12_ROOT_PARAMETER param[1] = {};
        param[0].InitAsDescriptorTable(_countof(ranges), ranges);

        D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);
        D3D12_ROOT_SIGNATURE_DESC sigdc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 1, &sampler);
        sigdc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> blobSignature;
        ComPtr<ID3DBlob> blobError;
        ::D3D12SerializeRootSignature(&sigdc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
        g_dx12Device.GetDevice()->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(),
                                     IID_PPV_ARGS(_rootSignature.GetAddressOf()));
    }
} // namespace tb