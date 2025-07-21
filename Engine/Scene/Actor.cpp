#include "Actor.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/PipelineStateHandler.h"
#include "Graphics/TextureResource.h"
#include "Graphics/GraphicsCore.h"
#include "Scene/Scene.h"

namespace tb
{
    Actor::Actor(Scene* scene)
    {
        // Base shader
        _cBuffer = new GpuBuffer;
        _cBuffer->CreateConstantBuffer(sizeof(Matrix), 256);

        scene->RegisterActor(this);
    }

    Actor::~Actor()
    {
        if (_cBuffer)
        {
            delete _cBuffer;
        }
    }

    void Actor::SetLocation(const Vector& location)
    {
        _transform._pos = location;
    }

    void Actor::Render(const XMMATRIX& vpMtx)
    {
        auto info = g_renderer.GetGeometryBuffer("Cube");

        g_dx12Device.GetCommmandList()->SetPipelineState(g_renderer.GetPipelineState("Cube").Get());

        // b0
        {
            FXMVECTOR posVec = DirectX::XMLoadFloat3(&_transform._pos);

            XMMATRIX rotMtx = XMMatrixRotationRollPitchYaw(_transform._rot.x, _transform._rot.y, _transform._rot.z);
            XMMATRIX translateMtx = XMMatrixTranslationFromVector(posVec);
            XMMATRIX worldMtx = rotMtx * translateMtx;
            XMMATRIX transposed = XMMatrixTranspose(worldMtx * vpMtx);
            XMStoreFloat4x4(&_matrix._wvpMat, transposed);

            int32 rootParamIndex = 0;

            void* buffer = &_matrix;
            uint32 size = sizeof(_matrix);

            // Get CB handler (PushData)
            ::memcpy(&_cBuffer->_mappedBuffer[_cBuffer->_currentIdx * _cBuffer->_elementSize], buffer, size);
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _cBuffer->_cpuHandleBegin, _cBuffer->_currentIdx * _cBuffer->_handleIncrementSize);
            _cBuffer->_currentIdx++;

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
        g_dx12Device.GetCommmandList()->IASetVertexBuffers(0, 1, &info->_vertexBufferView);
        g_dx12Device.GetCommmandList()->IASetIndexBuffer(&info->_indexBufferView);
        g_dx12Device.GetRootDescriptorHeap()->CommitTable(0); // CommitTable
        g_dx12Device.GetCommmandList()->DrawIndexedInstanced(info->_indexCount, 1, 0, 0, 0);
    }

    void Actor::Clear()
    {
        _cBuffer->_currentIdx = 0;
    }
} // namespace tb