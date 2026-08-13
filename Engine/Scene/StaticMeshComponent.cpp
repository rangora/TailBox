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

    void StaticMeshComponent::Render(const XMMATRIX& vpMtx)
    {
        if (!CheckResourceValidation())
        {
            return;
        }

        SceneComponent::Render(vpMtx);

        const Transform& parentTransform = _ownerActor->GetTrnasform();

        XMMATRIX parentTranslateMtx = XMMatrixTranslation(parentTransform._pos.x, parentTransform._pos.y, parentTransform._pos.z);
        XMMATRIX parentRotMtx = XMMatrixRotationRollPitchYaw(parentTransform._rot.x, parentTransform._rot.y, parentTransform._rot.z);
        XMFLOAT4X4 parentScale = Matrix::CreateScale(parentTransform._scale);
        XMMATRIX parentWorldMtx = XMLoadFloat4x4(&parentScale) * parentRotMtx * parentTranslateMtx;

        XMMATRIX translateMtx = XMMatrixTranslation(_transform._pos.x, _transform._pos.y, _transform._pos.z);
        XMMATRIX rotMtx = XMMatrixRotationRollPitchYaw(_transform._rot.x, _transform._rot.y, _transform._rot.z);
        XMFLOAT4X4 scale = Matrix::CreateScale(_transform._scale);
        XMMATRIX worldMtx = parentWorldMtx * (XMLoadFloat4x4(&scale) * rotMtx * translateMtx);

        DrawCall drawCall;
        drawCall._meshId = g_graphicsResources._cubeMesh->_VOI;
        drawCall._worldMtx = XMMatrixTranspose(worldMtx);
        drawCall._material = _renderResource.GetMaterial();

        g_renderQueue.Submit(drawCall);
    }

    void StaticMeshComponent::Clear()
    {
        SceneComponent::Clear();
    }
} // namespace tb