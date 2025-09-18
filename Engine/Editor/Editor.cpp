#include "Editor.h"
#include "Window/Window.h"
#include "Core/Input.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/CommandContext.h"
#include "Graphics/MemoryAllocator.h"
#include "Graphics/TextureResource.h"

namespace tb
{
    Editor::~Editor()
    {
        if (_window)
        {
            delete _window;
        }
    }

    void Editor::ShutDown()
    {
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

        TestFunc();
        ImGui::Begin("Scene");

        ImTextureID textureID = (ImTextureID)_gpuHandle.ptr;
        ImVec2 imageSize(512, 512);
        ImGui::Image(textureID, imageSize);

        ImGui::End();

        ImGui::ShowDemoWindow();
        ImGui::Render();
    }

    void Editor::CreateWinWindow(const WindowContext& _windowContext)
    {
        _window = new Window({"TailBox", VIEWPORT_WIDTH, VIEWPORT_HEIGHT});
    }

    void Editor::BindDevice(DX12Device* device)
    {
        g_commandContext._guiDescriptorPool->AllocDescriptor(&_cpuHandle, &_gpuHandle);

        _window->Initialize(device);
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

    void Editor::TestFunc()
    {
        TextureResource* texture = g_graphicsResources.GetTexture("niko");
        if (texture == nullptr)
        {
            return;
        }

        int32 descriptorSize =
            g_dx12Device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(_cpuHandle, 0, descriptorSize);
        g_dx12Device.GetDevice()->CopyDescriptorsSimple(1, srvDest, texture->_srvCpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

} // namespace tb