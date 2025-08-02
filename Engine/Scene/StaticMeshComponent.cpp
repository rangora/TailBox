#include "StaticMeshComponent.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/TextureResource.h"
#include "Graphics/Material.h"
#include "Graphics/ShaderResource.h"
#include "Actor.h"
#include "Engine.h"

namespace tb
{
    StaticMeshComponent::StaticMeshComponent(Actor* ownerActor) : SceneComponent(ownerActor)
    {
        _material = new Material;
        _material->InitializeDefaultProperties();
    }

    StaticMeshComponent::~StaticMeshComponent()
    {
        SceneComponent::~SceneComponent();
        if (_baseBuffer)
        {
            delete _baseBuffer;
        }
    }

    void StaticMeshComponent::SetStaticMesh(const std::string& meshName)
    {
        _geoemtryBuffer = g_renderer.GetGeometryBuffer(meshName);
        if (_geoemtryBuffer == nullptr)
        {
            return;
        }

        // new BaseBuffer
        _baseBuffer = new GpuBuffer;
        _baseBuffer->CreateConstantBuffer(sizeof(BaseConstants), 256);

        _meshName = meshName;
    }

    void StaticMeshComponent::Render(const XMMATRIX& vpMtx)
    {
        SceneComponent::Render(vpMtx);

        g_dx12Device.GetCommmandList()->SetPipelineState(g_renderer.GetPipelineState("Material").Get());

        Transform parentTransform = _ownerActor->GetTrnasform();
        Vector3 calcPos = parentTransform._pos + _transform._pos;

        FXMVECTOR posVec = DirectX::XMLoadFloat3(&calcPos);

        XMMATRIX rotMtx = XMMatrixRotationRollPitchYaw(_transform._rot.x, _transform._rot.y, _transform._rot.z);
        XMMATRIX translateMtx = XMMatrixTranslationFromVector(posVec);
        XMMATRIX worldMtx = rotMtx * translateMtx;

        BaseConstants constantBuffers;
        XMStoreFloat4x4(&constantBuffers._mesh._worldMtx, worldMtx);

        auto projMtx = Engine::GetActiveProjectionMatrix();
        auto viewMtx = Engine::GetActiveViewMatrix();
        XMMATRIX viewProjMtx = viewMtx * projMtx;
        XMStoreFloat4x4(&constantBuffers._global._viewProj, XMMatrixTranspose(viewMtx * projMtx));
        constantBuffers._global._cameraPosition = Engine::GetActiveCameraPosition();
        constantBuffers._global._time = 0.f;
        _material->UpdateMaterialConstantBuffer(constantBuffers._material);

        // b0
        int32 rootParamIndex = 0;
        {
            memcpy(&_baseBuffer->_mappedBuffer[_baseBuffer->_currentIdx * _baseBuffer->_elementSize],
                   &constantBuffers._mesh, sizeof(constantBuffers._mesh));

            D3D12_CPU_DESCRIPTOR_HANDLE sourceCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _baseBuffer->_cpuHandleBegin, _baseBuffer->_currentIdx * _baseBuffer->_handleIncrementSize);

            D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            g_dx12Device.GetRootDescriptorHeap()->SetCBV(sourceCpuHandle, destCpuHandle);
            _baseBuffer->_currentIdx++;
        }
        // b1
        {
            rootParamIndex = 1;
            memcpy(&_baseBuffer->_mappedBuffer[_baseBuffer->_currentIdx * _baseBuffer->_elementSize],
                   &constantBuffers._global, sizeof(constantBuffers._global));
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _baseBuffer->_cpuHandleBegin, _baseBuffer->_currentIdx * _baseBuffer->_handleIncrementSize);

            D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            g_dx12Device.GetRootDescriptorHeap()->SetCBV(sourceCpuHandle, destCpuHandle);
            _baseBuffer->_currentIdx++;
        }
        // b2
        {
            rootParamIndex = 2;
            memcpy(&_baseBuffer->_mappedBuffer[_baseBuffer->_currentIdx * _baseBuffer->_elementSize],
                   &constantBuffers._material, sizeof(constantBuffers._material));
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _baseBuffer->_cpuHandleBegin, _baseBuffer->_currentIdx * _baseBuffer->_handleIncrementSize);

            D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            g_dx12Device.GetRootDescriptorHeap()->SetCBV(sourceCpuHandle, destCpuHandle);
            _baseBuffer->_currentIdx++;
        }

        // t0
        {
            D3D12_CPU_DESCRIPTOR_HANDLE srvDestCpuHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(3);
            auto sampler = g_renderer.GetTexture("Niko");
            g_dx12Device.GetRootDescriptorHeap()->SetSRV(sampler->_srvHandle, srvDestCpuHandle);
        }

        g_dx12Device.GetCommmandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_dx12Device.GetCommmandList()->IASetVertexBuffers(0, 1, &_geoemtryBuffer->_vertexBufferView);
        g_dx12Device.GetCommmandList()->IASetIndexBuffer(&_geoemtryBuffer->_indexBufferView);
        g_dx12Device.GetRootDescriptorHeap()->CommitTable(0); // CommitTable
        g_dx12Device.GetCommmandList()->DrawIndexedInstanced(_geoemtryBuffer->_indexCount, 1, 0, 0, 0);
    }

    void StaticMeshComponent::Clear()
    {
        SceneComponent::Clear();
        _baseBuffer->_currentIdx = 0;
    }
} // namespace tb