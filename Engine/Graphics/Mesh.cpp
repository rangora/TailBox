#include "Mesh.h"
#include "Engine.h"
#include "DX12Device.h"
#include "GpuBuffer.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/Renderer.h"

namespace tb
{
    Mesh::Mesh(std::vector<Vertex>& vertexBuffer, std::vector<uint32>& indexBuffer)
    {
        _gBuffer = new GpuBuffer;
        _gBuffer->CreateConstantBuffer(sizeof(Transform_2), 256);

        /*
        // vertex buffer
        {
            uint32 vertexCount = static_cast<uint32>(vertexBuffer.size());
            uint32 bufferSize = vertexCount * sizeof(Vertex);

            UploadBuffer vBuffer;
            vBuffer.Create(bufferSize);
            void* mem = vBuffer.Map();
            ::memcpy(mem, &vertexBuffer[0], bufferSize);
            vBuffer.Unmap(0, bufferSize);

            _vBuffer.Create(L"VertexBuffer", bufferSize, &vBuffer, vertexBuffer);

            _vBuffer._vertexBufferView.BufferLocation = _vBuffer._gpuVirtualAddress;
            _vBuffer._vertexBufferView.StrideInBytes = sizeof(Vertex);
            _vBuffer._vertexBufferView.SizeInBytes = bufferSize;
        }

        // index buffer
        {
            _vBuffer._indexCount = static_cast<uint32>(indexBuffer.size());
            uint32 bufferSize = _vBuffer._indexCount * sizeof(uint32);

            D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

            Engine::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                                         nullptr, IID_PPV_ARGS(&_vBuffer._indexBuffer));

            void* indexDataBuffer = nullptr;
            CD3DX12_RANGE readRange(0, 0);
            _vBuffer._indexBuffer->Map(0, &readRange, &indexDataBuffer);
            ::memcpy(indexDataBuffer, &indexBuffer[0], bufferSize);
            _vBuffer._indexBuffer->Unmap(0, nullptr);

            _vBuffer._indexBufferView.BufferLocation = _vBuffer._indexBuffer->GetGPUVirtualAddress();
            _vBuffer._indexBufferView.Format = DXGI_FORMAT_R32_UINT;
            _vBuffer._indexBufferView.SizeInBytes = bufferSize;
        }
        */
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