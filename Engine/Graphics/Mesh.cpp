#include "Mesh.h"
#include "GpuBuffer.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GraphicsCore.h"
#include "Scene/Scene.h"

namespace tb
{
    Mesh::Mesh(Scene* scene)
    {
        _gBuffer = new GpuBuffer;
        _gBuffer->CreateConstantBuffer(sizeof(Transform_2), 256);

        // trnasformBuffer
        _transformBuffer = new GpuBuffer;
        _transformBuffer->CreateConstantBuffer(sizeof(Transform), 256);

        scene->AddMesh(this);

        _transform_old._offset = Vector4D(-0.4f, 0.f, 0.f, 0.f);
        _transform._pos = Vector(0.4f, 0.f, 0.f);
    }

    Mesh::~Mesh()
    {
        if (_gBuffer)
        {
            delete _gBuffer;
        }

        if (_transformBuffer)
        {
            delete _transformBuffer;
        }
    }

    void Mesh::Render(const XMMATRIX& vpMtx)
{
        auto info = g_renderer.GetGeometryBuffer("Box");

        g_dx12Device.GetCommmandList()->SetPipelineState(g_renderer.GetPipelineState("Cube").Get());

        // b1
        {
            int32 rootParamIndex = 0;
            void* buffer = &_transform_old;
            uint32 size = sizeof(_transform_old);

            // Get CB handler (PushData)
            ::memcpy(&_gBuffer->_mappedBuffer[_gBuffer->_currentIdx * _gBuffer->_elementSize], buffer, size);
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _gBuffer->_cpuHandleBegin, _gBuffer->_currentIdx * _gBuffer->_handleIncrementSize);
            _gBuffer->_currentIdx++;

            D3D12_CPU_DESCRIPTOR_HANDLE destCPUHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            g_dx12Device.GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);
        }
        // b2
        {
            int32 rootParamIndex = 1;
            void* buffer = &_transform;
            uint32 size = sizeof(_transform);

            // Get CB handler (PushData)
            ::memcpy(&_transformBuffer->_mappedBuffer[_transformBuffer->_currentIdx * _transformBuffer->_elementSize],
                     buffer, size);
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCPUHandle =
                CD3DX12_CPU_DESCRIPTOR_HANDLE(_transformBuffer->_cpuHandleBegin,
                                              _transformBuffer->_currentIdx * _transformBuffer->_handleIncrementSize);
            _transformBuffer->_currentIdx++;

            D3D12_CPU_DESCRIPTOR_HANDLE destCPUHandle =
                g_dx12Device.GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            g_dx12Device.GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);
        }

        // cbv push 다 하고
        // IASetPrimitiveTopology
        // IASetVertexBuffers
        // IASetIndexBuffer
        g_dx12Device.GetCommmandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_dx12Device.GetCommmandList()->IASetVertexBuffers(0, 1, &info->_vertexBufferView);
        g_dx12Device.GetCommmandList()->IASetIndexBuffer(&info->_indexBufferView);
        g_dx12Device.GetRootDescriptorHeap()->CommitTable(0);  // CommitTable
        g_dx12Device.GetCommmandList()->DrawIndexedInstanced(info->_indexCount, 1, 0, 0, 0);
    }

    void Mesh::Clear()
    {
        _gBuffer->_currentIdx = 0;
        _transformBuffer->_currentIdx = 0;
    }
} // namespace tb