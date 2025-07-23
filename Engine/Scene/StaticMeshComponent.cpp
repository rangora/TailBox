#include "StaticMeshComponent.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/TextureResource.h"
#include "Actor.h"

namespace tb
{
    StaticMeshComponent::StaticMeshComponent(Actor* ownerActor) : SceneComponent(ownerActor)
    {
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

        // create buffer
        _baseBuffer = new GpuBuffer;
        _baseBuffer->CreateConstantBuffer(sizeof(Matrix), 256);

        _meshName = meshName;
    }

    void StaticMeshComponent::Render(const XMMATRIX& vpMtx)
    {
        SceneComponent::Render(vpMtx);

         g_dx12Device.GetCommmandList()->SetPipelineState(g_renderer.GetPipelineState("Cube").Get());

        // b0
        {
            Transform parentTransform = _ownerActor->GetTrnasform();
            SimpleMath::Vector3 result = parentTransform._pos + _transform._pos;

            FXMVECTOR posVec = DirectX::XMLoadFloat3(&result); // 확인 필!

            XMMATRIX rotMtx = XMMatrixRotationRollPitchYaw(_transform._rot.x, _transform._rot.y, _transform._rot.z);
            XMMATRIX translateMtx = XMMatrixTranslationFromVector(posVec);
            XMMATRIX worldMtx = rotMtx * translateMtx;
            XMMATRIX transposed = XMMatrixTranspose(worldMtx * vpMtx);
            XMStoreFloat4x4(&_matrix._wvpMat, transposed);

            int32 rootParamIndex = 0;

            void* buffer = &_matrix;
            uint32 size = sizeof(_matrix);

            // Get CB handler (PushData)
            ::memcpy(&_baseBuffer->_mappedBuffer[_baseBuffer->_currentIdx * _baseBuffer->_elementSize], buffer, size);
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _baseBuffer->_cpuHandleBegin, _baseBuffer->_currentIdx * _baseBuffer->_handleIncrementSize);
            _baseBuffer->_currentIdx++;

            D3D12_CPU_DESCRIPTOR_HANDLE destCPUHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);

            g_dx12Device.GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);

            /*
             * srv start는 5(t0)
             * handle, 5
             */
            D3D12_CPU_DESCRIPTOR_HANDLE srvDestCPUHandle = g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(5);
            auto sampler = g_renderer.GetTexture("Niko");
            g_dx12Device.GetRootDescriptorHeap()->SetSRV(sampler->_srvHandle, srvDestCPUHandle);
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