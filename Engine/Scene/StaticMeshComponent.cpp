#include "StaticMeshComponent.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/TextureResource.h"
#include "Graphics/Material.h"
#include "Graphics/ShaderResource.h"
#include "Graphics/MemoryAllocator.h"
#include "Actor.h"
#include "Engine.h"
#include "Graphics/StaticMesh.h"

namespace tb
{
    StaticMeshComponent::StaticMeshComponent(Actor* ownerActor) : SceneComponent(ownerActor)
    {

    }

    StaticMeshComponent::~StaticMeshComponent()
    {
        SceneComponent::~SceneComponent();
    }

    void StaticMeshComponent::Release()
    {
        SceneComponent::Release();
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
        if (!_renderResource.LinkGeometryResources(meshName))
        {
            return;
        }

        _meshName = meshName;

        _staticMesh = g_graphicsResources._cubeMesh;
    }

    void StaticMeshComponent::Render(const XMMATRIX& vpMtx, ID3D12GraphicsCommandList* cmdList)
    {
        if (!CheckResourceValidation())
        {
            return;
        }

        SceneComponent::Render(vpMtx, cmdList);

        cmdList->SetPipelineState(g_renderer.GetPipelineState("Material").Get());

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

        MeshConstants meshConst;
        XMStoreFloat4x4(&meshConst._worldMtx, worldMtx);

        GlobalConstants globalConst;
        auto projMtx = Engine::GetActiveProjectionMatrix();
        auto viewMtx = Engine::GetActiveViewMatrix();
        XMMATRIX viewProjMtx = viewMtx * projMtx;
        XMStoreFloat4x4(&globalConst._viewProj, XMMatrixTranspose(viewProjMtx));
        globalConst._cameraPosition = Engine::GetActiveCameraPosition();
        globalConst._time = 0.f;

        MaterialConstants materialConst = {};
        Material* material = _renderResource.GetMaterial();
        if (material)
        {
            material->UpdateMaterialConstantBuffer(materialConst);
        }

        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

        if (!g_commandContext._descriptorPool->AllocDescriptor(&cpuHandle, &gpuHandle, 1))
        {
            return;
        }

        CBBlock* meshCB = g_commandContext._constantBufferPool->Allocate();
        CBBlock* globalCB = g_commandContext._constantBufferPool->Allocate();
        CBBlock* materialCB = g_commandContext._constantBufferPool->Allocate();
        if (!meshCB || !globalCB || !materialCB)
        {
            return;
        }

        *reinterpret_cast<MeshConstants*>(meshCB->_cpuMemAddr) = meshConst;
        *reinterpret_cast<GlobalConstants*>(globalCB->_cpuMemAddr) = globalConst;
        *reinterpret_cast<MaterialConstants*>(materialCB->_cpuMemAddr) = materialConst;

        cmdList->SetGraphicsRootConstantBufferView(0, meshCB->_gpuMemAddr);
        cmdList->SetGraphicsRootConstantBufferView(1, globalCB->_gpuMemAddr);
        cmdList->SetGraphicsRootConstantBufferView(2, materialCB->_gpuMemAddr);

        ID3D12DescriptorHeap* descriptorHeap = g_commandContext._descriptorPool->GetDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &descriptorHeap);

        if (material)
        {
            TextureResource* textureResource = material->GetTextureResource(TextureType::BASECOLOR);
            if (textureResource && textureResource->_srvCpuHandle.ptr)
            {
                g_renderAPI->GetDevice()->CopyDescriptorsSimple(1, cpuHandle, textureResource->_srvCpuHandle,
                                                                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
        }

        g_graphicsResources._cubeMesh->_VOI;
        auto geometryBuffer = _renderResource.GetGeometryBuffer();
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->SetGraphicsRootDescriptorTable(3, gpuHandle);

        g_renderAPI->Draw(g_graphicsResources._cubeMesh->_VOI, cmdList);
    }

    void StaticMeshComponent::Clear()
    {
        SceneComponent::Clear();
    }
} // namespace tb