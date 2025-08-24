#include "StaticMeshComponent.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/TextureResource.h"
#include "Graphics/Material.h"
#include "Graphics/ShaderResource.h"
#include "Graphics/MemoryAllocator.h"
#include "Actor.h"
#include "Engine.h"

namespace tb
{
    StaticMeshComponent::StaticMeshComponent(Actor* ownerActor) : SceneComponent(ownerActor)
    {
    }

    StaticMeshComponent::~StaticMeshComponent()
    {
        SceneComponent::~SceneComponent();
    }

    bool StaticMeshComponent::CheckResourceValidation() const
    {
        if (!_renderResource.GetGeometryBuffer())
        {
            return false;
        }

        return true;
    }

    void StaticMeshComponent::SetStaticMesh(const std::string& meshName)
    {
        if (_renderResource.LinkGeometryResources(meshName))
        {
            return;
        }

        _meshName = meshName;
    }

    void StaticMeshComponent::Render(const XMMATRIX& vpMtx)
    {
        if (!CheckResourceValidation())
        {
            return;
        }

        auto geometryBuffer = _renderResource.GetGeometryBuffer();

        SceneComponent::Render(vpMtx);

        g_dx12Device.GetCommmandList()->SetPipelineState(g_renderer.GetPipelineState("Material").Get());

        const Transform& parentTransform = _ownerActor->GetTrnasform();

        XMMATRIX parentTranslateMtx = XMMatrixTranslation(parentTransform._pos.x, parentTransform._pos.y, parentTransform._pos.z);
        XMMATRIX parentRotMtx = XMMatrixRotationRollPitchYaw(parentTransform._rot.x, parentTransform._rot.y, parentTransform._rot.z);
        XMFLOAT4X4 parentScale = Matrix::CreateScale(parentTransform._scale);
        XMMATRIX parentWorldMtx = XMLoadFloat4x4(&parentScale) * parentRotMtx * parentTranslateMtx;


        XMMATRIX translateMtx = XMMatrixTranslation(_transform._pos.x, _transform._pos.y, _transform._pos.z);
        XMMATRIX rotMtx = XMMatrixRotationRollPitchYaw(_transform._rot.x, _transform._rot.y, _transform._rot.z);
        XMFLOAT4X4 scale = Matrix::CreateScale(_transform._scale);
        XMMATRIX worldMtx = parentWorldMtx * (XMLoadFloat4x4(&scale) * rotMtx * translateMtx);

        worldMtx = XMMatrixTranspose(worldMtx);

        BaseConstants constantBuffers;
        XMStoreFloat4x4(&constantBuffers._mesh._worldMtx, worldMtx);

        auto projMtx = Engine::GetActiveProjectionMatrix();
        auto viewMtx = Engine::GetActiveViewMatrix();
        XMMATRIX viewProjMtx = viewMtx * projMtx;
        XMStoreFloat4x4(&constantBuffers._global._viewProj, XMMatrixTranspose(viewProjMtx));
        constantBuffers._global._cameraPosition = Engine::GetActiveCameraPosition();
        constantBuffers._global._time = 0.f;

        auto material = _renderResource.GetMaterial();
        if (material)
        {
            material->UpdateMaterialConstantBuffer(constantBuffers._material);
        }

        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

        if (!g_commandContext._descriptorPool->AllocDescriptor(&cpuHandle, &gpuHandle, 2))
        {
            return;
        }

        CBBlock* cbBlock = g_commandContext._constantBufferPool->Allocate();
        if (!cbBlock)
        {
            return;
        }

        int32 registerIndex = 0;
        int32 descriptorSize = g_dx12Device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        TempConstants* meshBuffer = reinterpret_cast<TempConstants*>(cbBlock->_cpuMemAddr);
        meshBuffer->_worldMtx = constantBuffers._mesh._worldMtx;
        meshBuffer->_viewProj = constantBuffers._global._viewProj;
        meshBuffer->_cameraPosition = constantBuffers._global._cameraPosition;
        meshBuffer->_time = constantBuffers._global._time;
        meshBuffer->_diffuse = constantBuffers._material._diffuse;
        meshBuffer->_specular = constantBuffers._material._specular;
        meshBuffer->_ambient = constantBuffers._material._ambient;
        meshBuffer->_emissive = constantBuffers._material._emissive;

        ID3D12DescriptorHeap* descriptorHeap = g_commandContext._descriptorPool->GetDescriptorHeap();
        g_dx12Device.GetCommmandList()->SetDescriptorHeaps(1, &descriptorHeap);

        CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(cpuHandle, registerIndex, descriptorSize);

        g_dx12Device.GetDevice()->CopyDescriptorsSimple(1, cbvDest, cbBlock->_handle,
                                                        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // tex
        TextureResource* textureResource = g_graphicsResources.GetTexture("niko");
        if (textureResource->_srvHandle.ptr)
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(cpuHandle, 1, descriptorSize);
            g_dx12Device.GetDevice()->CopyDescriptorsSimple(1, srvDest, textureResource->_srvHandle,
                                                            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        g_dx12Device.GetCommmandList()->SetGraphicsRootDescriptorTable(0, gpuHandle);
        g_dx12Device.GetCommmandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_dx12Device.GetCommmandList()->IASetVertexBuffers(0, 1, &geometryBuffer->_vertexBufferView);
        g_dx12Device.GetCommmandList()->IASetIndexBuffer(&geometryBuffer->_indexBufferView);
        g_dx12Device.GetCommmandList()->DrawIndexedInstanced(geometryBuffer->_indexCount, 1, 0, 0, 0);
    }

    void StaticMeshComponent::Clear()
    {
        SceneComponent::Clear();
    }
} // namespace tb