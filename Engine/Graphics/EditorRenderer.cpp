#include "EditorRenderer.h"
#include "GraphicsCore.h"
#include "D3D12RenderAPI.h"
#include "Editor/imgui/imgui.h"
#include "Editor/imgui/imgui_impl_dx12.h"
#include "CommandContext.h"
#include "Graphics/MemoryAllocator.h"

namespace tb
{
    EditorRenderer* EditorRenderer::_instance = nullptr;

    void EditorRenderer::Create()
    {
        _instance = new EditorRenderer;
    }

    void EditorRenderer::Initialize()
    {
        _renderAPI = g_renderAPI;
        _renderAPI->_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                    IID_PPV_ARGS(_commandAllocator.GetAddressOf()));
        _renderAPI->_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr,
                                               IID_PPV_ARGS(_commandList.GetAddressOf()));
        _commandList->Close();
    }

    void EditorRenderer::RenderBegin()
    {
    }

    void EditorRenderer::Render()
    {
        _commandList->Reset(_commandAllocator.Get(), nullptr);

        auto rtvHandle = _renderAPI->GetCurrentRTVHandle();
        auto dsvHandle = _renderAPI->GetDSVHandle();

        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        if (platform_io.Viewports.Data)
        {
            float width = platform_io.Viewports[0]->DrawData->DisplaySize.x;
            float height = platform_io.Viewports[0]->DrawData->DisplaySize.y;

            D3D12_VIEWPORT viewport = {0.f, 0.f, width, height, 0.f, 1.f};
            D3D12_RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
            _commandList->RSSetViewports(1, &viewport);
            _commandList->RSSetScissorRects(1, &rect);
        }

        _commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        ID3D12DescriptorHeap* descHeaps[] = {g_commandContext._guiDescriptorPool->GetDescriptorHeap()};
        _commandList->SetDescriptorHeaps(_countof(descHeaps), descHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());

        _commandList->Close();

        ID3D12CommandList* cmdLists[] = {_commandList.Get()};
        _renderAPI->GetCommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
    }

    void EditorRenderer::RenderEnd()
    {
    }

    void EditorRenderer::Flush()
    {
        _commandList->Reset(_commandAllocator.Get(), nullptr);
    }

    void EditorRenderer::Signal()
    {
    }

    EditorRenderer::EditorRenderer()
    {
    }

    EditorRenderer::~EditorRenderer()
    {
    }

} // namespace tb
