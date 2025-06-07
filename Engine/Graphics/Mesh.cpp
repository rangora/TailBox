#include "Mesh.h"
#include "Engine.h"
#include "DX12Device.h"
#include "GpuBuffer.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/Renderer.h"
#include "Scene/SceneManager.h"

namespace tb
{
    Mesh::Mesh()
    {
        _gBuffer = new GpuBuffer;
        _gBuffer->CreateConstantBuffer(sizeof(Transform_2), 256);

        // trnasformBuffer
        _transformBuffer = new GpuBuffer;
        _transformBuffer->CreateConstantBuffer(sizeof(Transform), 256);

        auto sceneMgr = SceneManager::Get();
        sceneMgr->RegisterMesh(this);


        _transform_old._offset = Vector4D(-0.4f, 0.f, 0.f, 0.f);
        _transform._pos = Vector4D(0.4f, 0.f, 0.f, 0.f);
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
        auto renderer = Renderer::Get();
        auto info = renderer->GetGeometryBuffer("Box");

        Engine::GetDX12Device()->GetCommmandList()->SetPipelineState(Renderer::Get()->GetShader("Box")->_pipelineState.Get());

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
                Engine::GetDX12Device()->GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            Engine::GetDX12Device()->GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);
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
                Engine::GetDX12Device()->GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            Engine::GetDX12Device()->GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);
        }

        // cbv push ´Ù ÇÏ°í
        // IASetPrimitiveTopology
        // IASetVertexBuffers
        // IASetIndexBuffer
        Engine::GetDX12Device()->GetCommmandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Engine::GetDX12Device()->GetCommmandList()->IASetVertexBuffers(0, 1, &info->_vertexBufferView);
        Engine::GetDX12Device()->GetCommmandList()->IASetIndexBuffer(&info->_indexBufferView);
        Engine::GetDX12Device()->GetRootDescriptorHeap()->CommitTable(0);  // CommitTable
        Engine::GetDX12Device()->GetCommmandList()->DrawIndexedInstanced(info->_indexCount, 1, 0, 0, 0);
    }

    void Mesh::Clear()
    {
        _gBuffer->_currentIdx = 0;
        _transformBuffer->_currentIdx = 0;
    }
} // namespace tb