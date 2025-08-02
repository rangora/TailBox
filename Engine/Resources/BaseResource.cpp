#include "BaseResource.h"
#include "Core.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/UploadBuffer.h"
#include "Graphics/GraphicsCore.h"

namespace tb
{
    namespace br
    {
        std::unique_ptr<GeometryBuffer> CreateCubeBuffer()
        {
            std::vector<Vertex> vertexVec = {
                // Front face
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},  // top-left
                {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},  // bottom-right
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // bottom-left
                {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},   // top-right

                // Right face
                {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // bottom-left
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},   // top-right
                {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},  // bottom-right
                {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},  // top-left

                // Left face
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},   // top-left
                {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // bottom-right
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},  // bottom-left
                {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},  // top-right

                // Back face
                {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},   // top-left
                {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // bottom-right
                {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},  // bottom-left
                {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},  // top-right

                // Top face
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // top-left
                {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},   // bottom-right
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},  // bottom-left
                {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},  // top-right

                // Bottom face
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},  // top-left
                {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},  // bottom-right
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // bottom-left
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}}    // top-right
            };

            std::vector<uint32> indexVec = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                            12, 13, 14, 12, 15, 13, 16, 18, 17, 16, 17, 19, 20, 22, 21, 20, 21, 23
            };


            std::unique_ptr<GeometryBuffer> geometryBuffer = std::make_unique<GeometryBuffer>();

            // vertex buffer
            {
                uint32 vertexCount = static_cast<uint32>(vertexVec.size());
                uint32 bufferSize = vertexCount * sizeof(Vertex);

                UploadBuffer vBuffer;
                vBuffer.Create(bufferSize);
                void* mem = vBuffer.Map();
                ::memcpy(mem, &vertexVec[0], bufferSize);
                vBuffer.Unmap(0, bufferSize);

                geometryBuffer->Create(L"Cube Vertex Buffer", bufferSize, &vBuffer, vertexVec);
                geometryBuffer->_vertexBufferView.BufferLocation = geometryBuffer->_gpuVirtualAddress;
                geometryBuffer->_vertexBufferView.StrideInBytes = sizeof(Vertex);
                geometryBuffer->_vertexBufferView.SizeInBytes = bufferSize;
            }

            // index buffer
            {
                geometryBuffer->_indexCount = static_cast<uint32>(indexVec.size());
                uint32 bufferSize = geometryBuffer->_indexCount * sizeof(uint32);

                D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

                g_dx12Device.GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                             D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                             IID_PPV_ARGS(&geometryBuffer->_indexBuffer));
                void* indexDataBuffer = nullptr;
                CD3DX12_RANGE readRange(0, 0);
                geometryBuffer->_indexBuffer->Map(0, &readRange, &indexDataBuffer);
                ::memcpy(indexDataBuffer, &indexVec[0], bufferSize);
                geometryBuffer->_indexBuffer->Unmap(0, nullptr);

                geometryBuffer->_indexBufferView.BufferLocation = geometryBuffer->_indexBuffer->GetGPUVirtualAddress();
                geometryBuffer->_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
                geometryBuffer->_indexBufferView.SizeInBytes = bufferSize;
            }

            return geometryBuffer;
        }
        std::unique_ptr<GeometryBuffer> CreateBoxBuffer()
        {
            std::vector<Vertex> Vertexvec(4);
            Vertexvec[0]._pos = Vector3(-0.5f, 0.5f, 0.5f);
            Vertexvec[1]._pos = Vector3(0.5f, 0.5f, 0.5f);
            Vertexvec[2]._pos = Vector3(0.5f, -0.5f, 0.5f);
            Vertexvec[3]._pos = Vector3(-0.5f, -0.5f, 0.5f);

            std::vector<uint32> indexVec;
            {
                indexVec.push_back(0);
                indexVec.push_back(1);
                indexVec.push_back(2);
            }
            {
                indexVec.push_back(0);
                indexVec.push_back(2);
                indexVec.push_back(3);
            }

            std::unique_ptr<GeometryBuffer> geometryBuffer = std::make_unique<GeometryBuffer>();

            // vertex buffer
            {
                uint32 vertexCount = static_cast<uint32>(Vertexvec.size());
                uint32 bufferSize = vertexCount * sizeof(Vertex);

                UploadBuffer vBuffer;
                vBuffer.Create(bufferSize);
                void* mem = vBuffer.Map();
                ::memcpy(mem, &Vertexvec[0], bufferSize);
                vBuffer.Unmap(0, bufferSize);

                geometryBuffer->Create(L"VertexBuffer", bufferSize, &vBuffer, Vertexvec);
                geometryBuffer->_vertexBufferView.BufferLocation = geometryBuffer->_gpuVirtualAddress;
                geometryBuffer->_vertexBufferView.StrideInBytes = sizeof(Vertex);
                geometryBuffer->_vertexBufferView.SizeInBytes = bufferSize;
            }

            // index buffer
            {
                geometryBuffer->_indexCount = static_cast<uint32>(indexVec.size());
                uint32 bufferSize = geometryBuffer->_indexCount * sizeof(uint32);

                D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

                g_dx12Device.GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
                                                             D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                             IID_PPV_ARGS(&geometryBuffer->_indexBuffer));

                void* indexDataBuffer = nullptr;
                CD3DX12_RANGE readRange(0, 0);
                geometryBuffer->_indexBuffer->Map(0, &readRange, &indexDataBuffer);
                ::memcpy(indexDataBuffer, &indexVec[0], bufferSize);
                geometryBuffer->_indexBuffer->Unmap(0, nullptr);

                geometryBuffer->_indexBufferView.BufferLocation = geometryBuffer->_indexBuffer->GetGPUVirtualAddress();
                geometryBuffer->_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
                geometryBuffer->_indexBufferView.SizeInBytes = bufferSize;
            }

            return geometryBuffer;
        } // namespace br


       /* std::unique_ptr<tb::Texture> CreateNikoTexture()
        {
            std::unique_ptr<Texture> texture = std::make_unique<Texture>();
            texture->CreateTexture("");
        }*/

    } // namespace br
} // namespace tb
