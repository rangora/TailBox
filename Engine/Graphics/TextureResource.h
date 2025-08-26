#pragma once

#include "GpuResource.h"
#include "DirectXTex.h"
#include "DirectXTex.inl"

namespace tb
{
    class TextureResource : public GpuResource
    {
    public:
        TextureResource();
        TextureResource(const TextureResource&) = delete;

        ~TextureResource();

        void Destroy() final;
        void CreateTexture(const std::string& path);

        D3D12_CPU_DESCRIPTOR_HANDLE _srvHandle;
        ComPtr<ID3D12DescriptorHeap> _srvHeap = nullptr;
    };
} // namespace tb