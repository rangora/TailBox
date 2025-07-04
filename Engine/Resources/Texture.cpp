#include "Texture.h"
#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "spdlog/spdlog.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace tb
{
    bool ConvertSTBToDirectXTex(ScratchImage& image, unsigned char* data, int width, int height, int channels)
    {
        // 메타데이터 설정
        TexMetadata metadata = {};
        metadata.width = width;
        metadata.height = height;
        metadata.depth = 1;
        metadata.arraySize = 1;
        metadata.mipLevels = 1;
        metadata.miscFlags = 0;
        metadata.miscFlags2 = 0;
        metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA 형태
        metadata.dimension = TEX_DIMENSION_TEXTURE2D;

        // ScratchImage에 데이터 복사
        if (FAILED(image.Initialize(metadata)))
        {
            return false;
        }

        const Image* img = image.GetImage(0, 0, 0);
        memcpy(img->pixels, data, width * height * 4); // RGBA = 4bytes per pixel
        return true;
    }

    void Texture::CreateTexture(const std::string& path)
    {
        int w, h, ch;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, STBI_rgb_alpha);
        if (!data)
        {
            spdlog::error("Texture load failed {}", path);
            return;
        }


        if (!ConvertSTBToDirectXTex(_image, data, w, h, ch))
        {
            spdlog::error("Texture convert failed {}", path);
        }

        if (FAILED(::CreateTexture(Engine::Get().GetDevice(), _image.GetMetadata(), &_texture)))
        {
            spdlog::error("Texture convert failed {}", path);
            return;
        }

        std::vector<D3D12_SUBRESOURCE_DATA> subResources;
        if (FAILED(::PrepareUpload(Engine::Get().GetDevice(), _image.GetImages(), _image.GetImageCount(),
                                   _image.GetMetadata(), subResources)))
        {
            spdlog::error("Texture upload failed {}", path);
            return;
        }

        uint64 bufferSize = ::GetRequiredIntermediateSize(_texture.Get(), 0, static_cast<uint32>(subResources.size()));

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ComPtr<ID3D12Resource> textureUploadHeap;

        if (FAILED(Engine::Get().GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(textureUploadHeap.GetAddressOf()))))
        {
            spdlog::error("Texture resource create failed {}", path);
            return;
        }

        Engine::GetDX12Device()->Flush();
        ::UpdateSubresources(Engine::Get().GetDX12Device()->GetCommmandList(), _texture.Get(), textureUploadHeap.Get(),
                             0, 0, static_cast<unsigned int>(subResources.size()), subResources.data());

        Engine::GetDX12Device()->GetCommmandList()->Close();
        ID3D12CommandList* ppCommandLists[] = {Engine::GetDX12Device()->GetCommmandList()};
        Engine::GetDX12Device()->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);

        // 임시로 Singal 사용.. stage에 올리도록 해야함
        Engine::GetDX12Device()->Signal();

        CreateView();
    }

    void Texture::CreateView()
    {
        D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
        srvDesc.NumDescriptors = 1;
        srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        Engine::Get().GetDevice()->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&_srvHeap));

        _srvHandle = _srvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
        viewDesc.Format = _image.GetMetadata().format;
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        viewDesc.Texture2D.MipLevels = 1;
        Engine::Get().GetDevice()->CreateShaderResourceView(_texture.Get(), &viewDesc, _srvHandle);
    }

} // namespace tb
