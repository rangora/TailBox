#include "D3D12RenderAPI.h"
#include "GraphicsCore.h"
#include "Resources/BaseResource.h"

namespace tb
{
    D3D12RenderAPI::D3D12RenderAPI()
    {
        _device = g_dx12Device.GetDevice();

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        _device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));

        _VOs.resize(1024, nullptr);
        _idxDispenser.Initialize(1024);
    }

    D3D12RenderAPI::~D3D12RenderAPI()
    {
    }

    void D3D12RenderAPI::SetupStaticMesh(unsigned int& VOI, const std::vector<Vertex>& vertices,
                                         const std::vector<uint32>& indices)
    {
        VOI = AllocateVOIndex();
        D3D12VO* VO = GetVO(VOI);
        if (VO)
        {
            uint32 vertexCount = static_cast<uint32>(br::_cubeVertices.size());
            uint32 vertexBufferSize = vertexCount * sizeof(Vertex);
            VO->_vertexCount = vertexCount;
            VO->_vertexBuffer = CreateBuffer(vertexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                             D3D12_HEAP_TYPE_DEFAULT, false, true, br::_cubeVertices.data());
            VO->_vertexBufferView.SizeInBytes = vertexBufferSize;
            VO->_vertexBufferView.StrideInBytes = sizeof(Vertex);
            VO->_vertexBufferView.BufferLocation = VO->_vertexBuffer._gpuAddr;

            uint32 indexCount = static_cast<uint32>(br::_cubeIndices.size());
            uint32 indexBufferSize = static_cast<uint32>(br::_cubeIndices.size() * sizeof(uint32));
            VO->_indexCount = indexCount;
            VO->_indexBuffer = CreateBuffer(indexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                            D3D12_HEAP_TYPE_DEFAULT, false, true, br::_cubeIndices.data());
            VO->_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
            VO->_indexBufferView.SizeInBytes = indexBufferSize;
            VO->_indexBufferView.BufferLocation = VO->_indexBuffer._gpuAddr;
        }
    }

    void D3D12RenderAPI::Draw(uint32 VOI)
    {
        _drawCommands.emplace_back(VOI);

        g_dx12Device.GetCommmandList()->IASetVertexBuffers(0, 1, &_VOs[VOI]->_vertexBufferView);
        g_dx12Device.GetCommmandList()->IASetIndexBuffer(&_VOs[VOI]->_indexBufferView);
        g_dx12Device.GetCommmandList()->DrawIndexedInstanced(_VOs[VOI]->_indexCount, 1, 0, 0, 0);
    }

    int32 D3D12RenderAPI::AllocateVOIndex()
    {
        int32 idx = _idxDispenser.Allocate();
        if (idx == -1)
        {
            return -1;
        }

        if (!_VOs[idx])
        {
            _VOs[idx] = new D3D12VO;
        }

        return idx;
    }

    D3D12VO* D3D12RenderAPI::GetVO(int32 idx)
    {
        return _VOs[idx];
    }

    void D3D12RenderAPI::FreeVOIndex(int32 idx)
    {
        _idxDispenser.Free(idx);

        if (_VOs[idx])
        {
            delete _VOs[idx];
        }
    }

    D3D12Buffer D3D12RenderAPI::CreateBuffer(uint64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState,
                                             D3D12_HEAP_TYPE heapType, bool cpuAddr, bool gpuAddr, const void* data)
    {
        D3D12Buffer buffer;
        buffer._size = static_cast<uint32>(size);

        /*
        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Alignment = 0;
        bufferDesc.Width = size;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.SampleDesc.Quality = 0;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.Flags = flags;
        */

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

        if (FAILED(_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                    data == nullptr ? initState : D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                    IID_PPV_ARGS(buffer._buffer.GetAddressOf()))))
        {
            spdlog::error("Failed to create default heap buffer.");
            return buffer;
        }

        if (cpuAddr)
        {
            buffer._buffer->Map(0, nullptr, reinterpret_cast<void**>(&buffer._cpuAddr));
        }

        if (gpuAddr)
        {
            buffer._gpuAddr = buffer._buffer->GetGPUVirtualAddress();
        }

        if (data)
        {
            if (cpuAddr)
            {
                memcpy(buffer._cpuAddr, data, size);
            }
            else
            {
                CD3DX12_HEAP_PROPERTIES uploadProp(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
                ComPtr<ID3D12Resource> uploadBuffer = nullptr;
                if (FAILED(_device->CreateCommittedResource(&uploadProp, D3D12_HEAP_FLAG_NONE, &uploadDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                            IID_PPV_ARGS(uploadBuffer.GetAddressOf()))))
                {
                    spdlog::error("Failed to create default heap buffer.");
                    return buffer;
                }

                D3D12_SUBRESOURCE_DATA subData = {};
                subData.pData = data;
                subData.RowPitch = size;
                subData.SlicePitch = size;

                // 대체 필
                 g_dx12Device.Flush();
                UpdateSubresources(g_dx12Device.GetCommmandList(), buffer._buffer.Get(), uploadBuffer.Get(), 0, 0,
                                   1, &subData);

                CD3DX12_RESOURCE_BARRIER resourceBarrier =
                    CD3DX12_RESOURCE_BARRIER::Transition(buffer._buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

                g_dx12Device.GetCommmandList()->ResourceBarrier(1, &resourceBarrier);

                g_dx12Device.GetCommmandList()->Close();
                ID3D12CommandList* ppCommandLists[] = {g_dx12Device.GetCommmandList()};
                g_dx12Device.GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);
                g_dx12Device.Signal();
                // 대체
            }
        }

        return buffer;
    }

} // namespace tb