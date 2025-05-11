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

        auto sceneMgr = SceneManager::Get();
        sceneMgr->RegisterMesh(this);
    }

    Mesh::~Mesh()
    {
        if (_gBuffer)
        {
            delete _gBuffer;
        }
    }

    void Mesh::Render()
    {
        auto renderer = Renderer::Get();
        auto info = renderer->GetGeometryBuffer("Box");

        Engine::GetDX12Device()->GetCommmandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Engine::GetDX12Device()->GetCommmandList()->IASetVertexBuffers(0, 1, &info->_vertexBufferView);
        Engine::GetDX12Device()->GetCommmandList()->IASetIndexBuffer(&info->_indexBufferView);

        // CB
        int32 rootParamIndex = 0;
        void* buffer = &_transform;
        uint32 size = sizeof(_transform);

        // Get CB handler (PushData)
        ::memcpy(&_gBuffer->_mappedBuffer[_gBuffer->_currentIdx * _gBuffer->_elementSize], buffer, size);
        D3D12_CPU_DESCRIPTOR_HANDLE sourceCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            _gBuffer->_cpuHandleBegin, _gBuffer->_currentIdx * _gBuffer->_handleIncrementSize);
        _gBuffer->_currentIdx++;


        D3D12_CPU_DESCRIPTOR_HANDLE destCPUHandle = Engine::GetDX12Device()->GetRootDescriptorHeap()->GetCPUHandle(0);
        Engine::GetDX12Device()->GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);
        Engine::GetDX12Device()->GetRootDescriptorHeap()->CommitTable();
        Engine::GetDX12Device()->GetCommmandList()->DrawIndexedInstanced(info->_indexCount, 1, 0, 0, 0);
    }

    void Mesh::Clear()
    {
        _gBuffer->_currentIdx = 0;
    }
} // namespace tb