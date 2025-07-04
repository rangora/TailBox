#pragma once

#include "Core.h"
#include "DirectXTex.h"
#include "DirectXTex.inl"

namespace tb
{
    class Texture
    {
    public:
        void CreateTexture(const std::string& path);
        void CreateView();

        ScratchImage _image;
        ComPtr<ID3D12Resource> _texture;
        ComPtr<ID3D12DescriptorHeap> _srvHeap = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE _srvHandle;
    };
} // namespace tb