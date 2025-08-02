#include "Material.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GraphicsCore.h"

namespace tb
{
    Material::Material() = default;
    Material::~Material() = default;

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

    void Material::BindTextures()
    {
    }

    void Material::UpdateMaterialConstantBuffer(MaterialConstants& cBuffer)
    {
        cBuffer._ambient = _properties._ambient;
        cBuffer._diffuse = _properties._diffuse;
        cBuffer._emissive = _properties._emissive;
        cBuffer._specular = _properties._specular;
    }

} // namespace tb