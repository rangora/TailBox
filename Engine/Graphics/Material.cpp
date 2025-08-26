#include "Material.h"
#include "Graphics/GraphicsCore.h"

namespace tb
{
    Material::Material()
    {

    }

    Material::~Material()
    {
        Release();
    }

    void Material::InitializeDefaultProperties()
    {
        // TEMP
        _constantBuffer = std::make_unique<GpuBuffer>();
        _constantBuffer->CreateConstantBuffer(sizeof(MaterialProperties), 1);

        _properties._diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
        _properties._specular = {0.5f, 0.5f, 0.5f, 1.0f};
        _properties._ambient = {0.1f, 0.1f, 0.1f, 1.0f};
        _properties._emissive = {0.0f, 0.0f, 0.0f, 0.0f};
    }

    void Material::Release()
    {
        /*  for (TextureResource* ptr : _textureResources)
          {
              if (ptr)
              {
                  delete ptr;
              }
          }*/

        if (_constantBuffer)
        {
            _constantBuffer->Destroy();
            _constantBuffer.reset();
            _constantBuffer = nullptr;
        }
    }

    void Material::BindTextures(TextureType type, const std::string& textureId)
    {
        int32 idx = static_cast<int32>(type);

        TextureResource* foundTex = g_graphicsResources.GetTexture(textureId);
        if (!foundTex)
        {
            spdlog::warn("No textureId {0}", textureId);
            return;
        }

        _textureResources[idx] = foundTex;
    }

    void Material::UpdateMaterialConstantBuffer(MaterialConstants& cBuffer)
    {
        cBuffer._ambient = _properties._ambient;
        cBuffer._diffuse = _properties._diffuse;
        cBuffer._emissive = _properties._emissive;
        cBuffer._specular = _properties._specular;
    }

    TextureResource* Material::GetTextureResource(TextureType type)
    {
        int32 idx = static_cast<int32>(type);
        return _textureResources[idx];
    }

} // namespace tb