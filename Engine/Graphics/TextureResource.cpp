#include "TextureResource.h"
#include "GraphicsCore.h"
#include "spdlog/spdlog.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace tb
{
    bool ConvertStbToDirectXTex(ScratchImage& image, unsigned char* data, int width, int height, int channels)
    {
        TexMetadata metadata = {};
        metadata.width = width;
        metadata.height = height;
        metadata.depth = 1;
        metadata.arraySize = 1;
        metadata.mipLevels = 1;
        metadata.miscFlags = 0;
        metadata.miscFlags2 = 0;
        metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        metadata.dimension = TEX_DIMENSION_TEXTURE2D;

        if (FAILED(image.Initialize(metadata)))
        {
            return false;
        }

        const Image* img = image.GetImage(0, 0, 0);
        memcpy(img->pixels, data, width * height * 4); // RGBA = 4bytes per pixel
        return true;
    }

    TextureResource::TextureResource() = default;

    TextureResource::~TextureResource()
    {
        if (_srvHeap)
        {
            _srvHeap.Reset();
        }
    }

    void TextureResource::Destroy()
    {
    }

    void TextureResource::CreateTexture(const std::string& path)
    {
        int w, h, ch;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, STBI_rgb_alpha);
        if (!data)
        {
            spdlog::error("Texture load failed {}", path);
            return;
        }

        ScratchImage image;

        if (!ConvertStbToDirectXTex(image, data, w, h, ch))
        {
            spdlog::error("Texture convert failed {}", path);
        }

        if (FAILED(::CreateTexture(g_dx12Device.GetDevice(), image.GetMetadata(), &_resource)))
        {
            spdlog::error("Texture convert failed {}", path);
            return;
        }

        std::vector<D3D12_SUBRESOURCE_DATA> subResources;
        if (FAILED(::PrepareUpload(g_dx12Device.GetDevice(), image.GetImages(), image.GetImageCount(),
                                   image.GetMetadata(), subResources)))
        {
            spdlog::error("Texture upload failed {}", path);
            return;
        }

        uint64 bufferSize = ::GetRequiredIntermediateSize(_resource.Get(), 0, static_cast<uint32>(subResources.size()));

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ComPtr<ID3D12Resource> textureUploadHeap;

        if (FAILED(g_dx12Device.GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                                      IID_PPV_ARGS(textureUploadHeap.GetAddressOf()))))
        {
            spdlog::error("Texture resource create failed {}", path);
            return;
        }

        g_dx12Device.Flush();
        ::UpdateSubresources(g_dx12Device.GetCommmandList(), _resource.Get(), textureUploadHeap.Get(),
                             0, 0, static_cast<unsigned int>(subResources.size()), subResources.data());

        g_dx12Device.GetCommmandList()->Close();
        ID3D12CommandList* ppCommandLists[] = {g_dx12Device.GetCommmandList()};
        g_dx12Device.GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);

        // 임시로 Singal 사용.. stage에 올리도록 해야함
        g_dx12Device.Signal();

        // Create view
        D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
        srvDesc.NumDescriptors = 1;
        srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        g_dx12Device.GetDevice()->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&_srvHeap));

        _srvHandle = _srvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
        viewDesc.Format = image.GetMetadata().format;
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        viewDesc.Texture2D.MipLevels = 1;
        g_dx12Device.GetDevice()->CreateShaderResourceView(_resource.Get(), &viewDesc, _srvHandle);
    }

} // namespace tb