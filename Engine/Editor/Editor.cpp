#include "Editor.h"
#include "Window/Window.h"
#include "Core/Input.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/CommandContext.h"
#include "Graphics/MemoryAllocator.h"
#include "Graphics/TextureResource.h"
#include "Graphics/Utility/RenderTexture.h"
#include "Scene/SceneManager.h"
#include "Engine.h"
#include "../Graphics/EditorRenderer.h"
#include "../Graphics/D3D12RenderAPI.h"

namespace tb
{
    Editor::Editor()
    {
    }

    Editor::~Editor()
    {
        if (_window)
        {
            delete _window;
        }
    }

    void Editor::ShutDown()
    {
        if (_renderTexture)
        {
            _renderTexture->Release();
            _renderTexture.reset();
        }

        if (_rtvResource)
        {
            _rtvResource.Reset();
        }

        if (_textureRtvHeap)
        {
            _textureRtvHeap.Reset();
        }

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (_window)
        {
            _window->ShutdownWindow();
        }
    }

    void Editor::Update(float delta)
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ProcessKeyInput();

        _window->Update();

        TestFunc2();
        ImGui::Begin("Scene");

        ImTextureID textureID = (ImTextureID)_gpuHandle.ptr;
        ImVec2 imageSize(512, 512);
        ImGui::Image(textureID, imageSize);

        ImGui::End();

        //ImGui::ShowDemoWindow();
        ImGui::Render();
    }

    void Editor::CreateWinWindow(const WindowContext& _windowContext)
    {
        _window = new Window({"TailBox", VIEWPORT_WIDTH, VIEWPORT_HEIGHT});
    }

    void Editor::BindDevice()
    {
        EditorRenderer::Create();
        EditorRenderer::Get()->Initialize();

        g_commandContext._guiDescriptorPool->AllocDescriptor(&_cpuHandle, &_gpuHandle);

        _window->Initialize();

        // create rtv
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = BUFFERCOUNT;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        g_renderAPI->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_textureRtvHeap.GetAddressOf()));
        _rtvHandle = _textureRtvHeap->GetCPUDescriptorHandleForHeapStart();

        {
            CD3DX12_RESOURCE_DESC rtvDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 800, 600, 1, 1,
                                                                         1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
            CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            g_renderAPI->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &rtvDesc,
                                                              D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue,
                                                              IID_PPV_ARGS(_rtvResource.GetAddressOf()));

            g_renderAPI->GetDevice()->CreateRenderTargetView(_rtvResource.Get(), nullptr, _rtvHandle);
        }


        // set texture cpuHandle
        _renderTexture = std::make_unique<RenderTexture>(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
        _renderTexture->SetSrvHandle(_cpuHandle);
        _renderTexture->SetRtvHandle(_rtvHandle);
        _renderTexture->CreateResource(800, 600);
    }

    void Editor::CreateDefaultLayout()
    {
    }

    void Editor::ProcessKeyInput()
    {
        Input::ClearReleasedInput();
        Input::TransitionPressedButtons();

        ImGuiKey startKey = ImGuiKey_NamedKey_BEGIN;
        for (ImGuiKey key = startKey; key < ImGuiKey_NamedKey_END; key = static_cast<ImGuiKey>(key + 1))
        {
            if (key == ImGuiKey_MouseRight)
            {
                if (ImGui::IsKeyDown(key))
                {
                    if (!Input::IsMouseButtonDown(MouseButton::Right))
                    {
                        Input::UpdateMouseButtonState(MouseButton::Right, KeyState::Pressed);
                    }
                }
                else if (Input::IsMouseButtonDown(MouseButton::Right))
                {
                    Input::UpdateMouseButtonState(MouseButton::Right, KeyState::Released);
                }
            }

            if (key == ImGuiKey_W)
            {
                if (ImGui::IsKeyDown(key))
                {
                    if (!Input::IsKeyButtonDown(KeyButton::W))
                    {
                        Input::UpdateKeyButtonState(KeyButton::W, KeyState::Pressed);
                    }
                }
                else if (Input::IsKeyButtonDown(KeyButton::W))
                {
                    Input::UpdateKeyButtonState(KeyButton::W, KeyState::Released);
                }
            }

            if (key == ImGuiKey_S)
            {
                if (ImGui::IsKeyDown(key))
                {
                    if (!Input::IsKeyButtonDown(KeyButton::S))
                    {
                        Input::UpdateKeyButtonState(KeyButton::S, KeyState::Pressed);
                    }
                }
                else if (Input::IsKeyButtonDown(KeyButton::S))
                {
                    Input::UpdateKeyButtonState(KeyButton::S, KeyState::Released);
                }
            }

            if (key == ImGuiKey_A)
            {
                if (ImGui::IsKeyDown(key))
                {
                    if (!Input::IsKeyButtonDown(KeyButton::A))
                    {
                        Input::UpdateKeyButtonState(KeyButton::A, KeyState::Pressed);
                    }
                }
                else if (Input::IsKeyButtonDown(KeyButton::A))
                {
                    Input::UpdateKeyButtonState(KeyButton::A, KeyState::Released);
                }
            }

            if (key == ImGuiKey_D)
            {
                if (ImGui::IsKeyDown(key))
                {
                    if (!Input::IsKeyButtonDown(KeyButton::D))
                    {
                        Input::UpdateKeyButtonState(KeyButton::D, KeyState::Pressed);
                    }
                }
                else if (Input::IsKeyButtonDown(KeyButton::D))
                {
                    Input::UpdateKeyButtonState(KeyButton::D, KeyState::Released);
                }
            }
        }
    }

    void Editor::TestFunc2()
    {
        auto renderer = EditorRenderer::Get();
        auto renderAPI = g_renderAPI;

        ID3D12CommandList* ppCommandLists[] = {renderer->GetCommmandList()};

        renderer->Flush();
        if (_renderTexture->TransitionTo(renderer->GetCommmandList(), D3D12_RESOURCE_STATE_RENDER_TARGET))
        {
            // Draw
            {

                _renderTexture->Clear(renderer->GetCommmandList());
                g_commandContext.SetCommandList(renderer->GetCommmandList());
                Engine::Get().GetSceneManager()->Render();
            }

            renderer->GetCommmandList()->Close();
            renderAPI->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);
            renderAPI->Signal();
        }

        renderer->Flush();
        if (_renderTexture->TransitionTo(renderer->GetCommmandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE))
        {
            renderer->GetCommmandList()->Close();
            renderAPI->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);
            renderAPI->Signal();
        }
    }

} // namespace tb