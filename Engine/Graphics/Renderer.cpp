#include "Renderer.h"
#include "UploadBuffer.h"
#include "Engine.h"
#include "Scene/SceneManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"

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
        static std::array<std::string, 1> baseShaderNames = {"Box"};

        for (const auto name : baseShaderNames)
        {
            auto newShader = std::make_unique<Shader>(L"E:\\workspace\\TailBox\\Engine\\Resources\\default.hlsli",
                                                      L"E:\\workspace\\TailBox\\Engine\\Resources\\default.hlsli");

            _shaders.emplace(name, std::move(newShader));
        }

        {
            auto newShader = std::make_unique<Shader>(L"E:\\workspace\\TailBox\\Engine\\Resources\\cube.hlsli",
                                                      L"E:\\workspace\\TailBox\\Engine\\Resources\\cube.hlsli");
            _shaders.emplace("Cube", std::move(newShader));
        }
    }

    void Renderer::RenderFrame()
    {
        RenderBegin();
        Render();
        RenderEnd();
    }

    ID3D12GraphicsCommandList* Renderer::GetCommandList()
    {
        return Engine::GetDX12Device()->GetCommmandList();
    }

    DescriptorHeap* Renderer::GetRootDescriptorHeap()
    {
        return Engine::GetDX12Device()->GetRootDescriptorHeap();
    }

    void Renderer::StageBuffer(UploadBuffer* uploadBuffer)
    {
        Engine::GetDX12Device()->StageBuffer(uploadBuffer);
    }

    void Renderer::RenderBegin()
    {
        auto device = Engine::GetDX12Device();

        _nextFrameCtx = device->WaitForNextFrameResources();
        _backBufferIndex = device->GetSwapChain()->GetCurrentBackBufferIndex();
        _nextFrameCtx->_commandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = device->GetMainRtvResource(_backBufferIndex);
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        auto commandList = device->GetCommmandList();
        commandList->Reset(_nextFrameCtx->_commandAllocator, nullptr);

        commandList->SetGraphicsRootSignature(device->GetRootSignature());

        SceneManager::Get()->OnRenderBegin();
        device->GetRootDescriptorHeap()->Clear();

        ID3D12DescriptorHeap* mainHeap = device->GetRootDescriptorHeap()->GetDescriptorHeap();
        commandList->SetDescriptorHeaps(1, &mainHeap);

        commandList->ResourceBarrier(1, &barrier);

        const float clearColors[4] = {0.45f, 0.55f, 0.6f, 1.f};

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(device->GetDSDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        D3D12_VIEWPORT viewport = {};
        D3D12_RECT rect = {};
        if (platform_io.Viewports.Data)
        {
            viewport.Height = platform_io.Viewports[0]->DrawData->DisplaySize.y;
            viewport.Width = platform_io.Viewports[0]->DrawData->DisplaySize.x;
            rect.bottom = platform_io.Viewports[0]->DrawData->DisplaySize.y;
            rect.right = platform_io.Viewports[0]->DrawData->DisplaySize.x;
        }

        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &rect);

        commandList->ClearRenderTargetView(device->GetMainRtvCpuHandle(_backBufferIndex), clearColors, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
    }

    void Renderer::Render()
    {
        auto device = Engine::GetDX12Device();

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(device->GetDSDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
        device->GetCommmandList()->OMSetRenderTargets(1, &device->GetMainRtvCpuHandle(_backBufferIndex), FALSE, &dsvHandle);

        XMMATRIX vpMtx = device->_camera._viewMtx * device->_camera._projMtx;
        SceneManager::Get()->Render(vpMtx);

        RenderImGui();
    }

    void Renderer::RenderEnd()
    {
        auto device = Engine::GetDX12Device();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = device->GetMainRtvResource(_backBufferIndex);
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        device->GetCommmandList()->ResourceBarrier(1, &barrier);
        device->GetCommmandList()->Close();

        ID3D12CommandList* ppCommandLists[] = {device->GetCommmandList()};
        device->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT hr = device->GetSwapChain()->Present(1, 0);
        device->_bSwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

        uint64 fenceValue = device->_fenceLastSignalValue + 1;
        device->GetCommandQueue()->Signal(device->_fence.Get(), fenceValue);
        device->_fenceLastSignalValue = fenceValue;
        _nextFrameCtx->_fenceValue = fenceValue;
    }

    void Renderer::RenderImGui()
    {
        auto device = Engine::GetDX12Device();
        ID3D12DescriptorHeap* descHeaps[] = {device->GetImGuiDescHeap()};
        device->GetCommmandList()->SetDescriptorHeaps(_countof(descHeaps), descHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), device->GetCommmandList());
    }
} // namespace tb
