#pragma once

#include "Core.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "GpuBuffer.h"

namespace tb
{
    enum class TextureType
    {
        BASECOLOR,
        DIFFUSE,
        SPECULAR,
        AMBIENT,
        EMISSIVE,
        NORMAL,
        MAX_TEXTURE_TYPES
    };

    static constexpr int32 MAX_TEXTURE_TYTPE_SIZE = static_cast<int32>(TextureType::MAX_TEXTURE_TYPES);

    struct MaterialProperties
    {
        Vector4 _diffuse = {1.f, 1.f, 1.f, 1.f};
        Vector4 _specular = {0.5f, 0.5f, 0.5f, 1.f};
        Vector4 _ambient = {0.1f, 0.1f, 0.1f, 1.f};
        Vector4 _emissive = {0.f, 0.f, 0.f, 1.f};
    };

    class Material
    {
    public:
        Material();
        ~Material();
        void InitializeDefaultProperties();
        void Release();

        void BindTextures(TextureType type, const std::string& textureId);
        void UpdateMaterialConstantBuffer(MaterialConstants& cBuffer);
        TextureResource* GetTextureResource(TextureType type);

    private:
        std::string _name;
        MaterialProperties _properties;
        std::array<TextureResource*, MAX_TEXTURE_TYTPE_SIZE> _textureResources{};

        std::unique_ptr<GpuBuffer> _constantBuffer = nullptr;
    };
} // namespace tb