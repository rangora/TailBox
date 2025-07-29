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

    void Material::UpdateConstantBuffers()
    {
        if (_constantBuffer->_mappedBuffer)
        {
            g_dx12Device.GetCommmandList()->SetPipelineState(g_renderer.GetPipelineState("Material").Get());

            // Render..?
            memcpy(_constantBuffer->_mappedBuffer, &_properties, sizeof(MaterialProperties));

            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _constantBuffer->_cpuHandleBegin, _constantBuffer->_currentIdx * _constantBuffer->_handleIncrementSize);

            D3D12_CPU_DESCRIPTOR_HANDLE destHandle = g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(2);
            g_dx12Device.GetRootDescriptorHeap()->SetCBV(cpuHandle, destHandle);
        }
    }
} // namespace tb