#include "Renderer.h"
#include "DX12Device.h"
#include "Engine.h"
#include "PipelineStateHandler.h"
#include "UploadBuffer.h"

namespace tb
{
    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::Initialize()
    {
        _pipelineStateHandler = std::make_unique<PipelineStateHandler>();

        _geoemtryBuffers.reserve(100);
        _shaders.reserve(100);

        InitBuffers();
        InitShaders();

    }

    ComPtr<ID3D12PipelineState> Renderer::GetPipelineState(const std::string& identifier)
    {
        return _pipelineStateHandler->GetPipelineState(identifier);
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
        if (_geoemtryBuffers.find("Cube") == _geoemtryBuffers.end())
        {
            std::vector<Vertex> vertexVec = {// front face
                                             {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                             {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                             {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                                             {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                                             // right face
                                             {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                             {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                             {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                                             {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                                             // left face
                                             {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                             {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                             {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                                             {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                                             // back face
                                             {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                             {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                             {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                                             {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                                             // top face
                                             {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                             {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                             {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                                             {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                                             // bottom face
                                             {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                             {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                             {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                                             {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}}};

            std::vector<uint32> indexVec = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 24, 21};

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

            _geoemtryBuffers.emplace("Cube", std::move(geometryBuffer));
        }

    }

    void Renderer::InitShaders()
    {
        std::vector<ShaderCompileDesc> shaderCompileDescs;
        shaderCompileDescs.push_back({ShaderType::Vertex, tb::core::projectPath + "/Resources/default.hlsli", "Cube_VS"});
        shaderCompileDescs.push_back({ShaderType::Vertex, tb::core::projectPath + "/Resources/default.hlsli", "Cube_PS"});

       /* for (const auto& shaderCompileDesc : shaderCompileDescs)
        {
            std::make_unique<Shader>(shaderCompileDesc);
        }*/

        static std::array<std::string, 1> baseShaderNames = {"Box"};
        for (const auto name : baseShaderNames)
        {
            auto newShader = std::make_unique<Shader>(tb::core::projectPath + "/Resources/default.hlsli",
                                                      tb::core::projectPath + "/Resources/default.hlsli");

            _shaders.emplace(name, std::move(newShader));
        }

        {
            auto newShader = std::make_unique<Shader>(tb::core::projectPath + "/Resources/cube.hlsli",
                                                      tb::core::projectPath + "/Resources/cube.hlsli");
            _shaders.emplace("Cube", std::move(newShader));
        }

        GraphicsPipelineStateDesc pipelineStateDesc;
        pipelineStateDesc._identifier = "Cube";
        pipelineStateDesc._inputElements.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        pipelineStateDesc._inputElements.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        // blobs
        auto it = _shaders.find("Cube");
        pipelineStateDesc._desc.PS = {it->second.get()->_psBlob->GetBufferPointer(),
                                      it->second.get()->_psBlob->GetBufferSize()};
        pipelineStateDesc._desc.VS = {it->second.get()->_vsBlob->GetBufferPointer(),
                                      it->second.get()->_vsBlob->GetBufferSize()};

        pipelineStateDesc._desc.InputLayout = {pipelineStateDesc._inputElements.data(),
                                               static_cast<UINT>(pipelineStateDesc._inputElements.size())};
        pipelineStateDesc._desc.pRootSignature = Engine::GetDX12Device()->GetRootSignature();
        pipelineStateDesc._desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pipelineStateDesc._desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pipelineStateDesc._desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        pipelineStateDesc._desc.SampleMask = UINT_MAX;
        pipelineStateDesc._desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc._desc.NumRenderTargets = 1;
        pipelineStateDesc._desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pipelineStateDesc._desc.SampleDesc.Count = 1;
        pipelineStateDesc._desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

        _pipelineStateHandler->CreatePipelineState(pipelineStateDesc);
    }
}; // namespace tb