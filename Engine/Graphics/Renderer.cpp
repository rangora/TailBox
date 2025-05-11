#include "Renderer.h"
#include "UploadBuffer.h"
#include "Engine.h"

namespace tb
{
    void Renderer::Initialize()
    {
        _geoemtryBuffers.reserve(100);
        _shaders.reserve(100);

        InitBuffers();
        InitShaders();
    }

    void Renderer::InitBuffers()
    {
        if (_geoemtryBuffers.find("Box") == _geoemtryBuffers.end())
        {
            std::vector<Vertex> Vertexvec(4);
            Vertexvec[0]._pos = Vector(-0.5f, 0.5f, 0.5f);
            Vertexvec[0]._color = Vector4D(1.f, 0.f, 0.f, 1.f);
            Vertexvec[1]._pos = Vector(0.5f, 0.5f, 0.5f);
            Vertexvec[1]._color = Vector4D(0.f, 1.f, 0.f, 1.f);
            Vertexvec[2]._pos = Vector(0.5f, -0.5f, 0.5f);
            Vertexvec[2]._color = Vector4D(0.f, 0.f, 1.f, 1.f);
            Vertexvec[3]._pos = Vector(-0.5f, -0.5f, 0.5f);
            Vertexvec[3]._color = Vector4D(0.f, 1.f, 0.f, 1.f);

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

                Engine::GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
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

            _geoemtryBuffers.emplace("Box", std::move(geometryBuffer));
        }
    }

    void Renderer::InitShaders()
    {
        static std::array<std::string, 1> baseShaderNames = {"Box"};

        for (const auto name : baseShaderNames)
        {
            auto newShader = std::make_unique<Shader>(L"E:\\workspace\\TailBox\\Engine\\Resources\\default.hlsli",
                                                      L"E:\\workspace\\TailBox\\Engine\\Resources\\default.hlsli");

            _shaders.emplace(name, std::move(newShader));
        }
    }
}; // namespace tb