#include "EditorRenderer.h"
#include "D3D12RenderAPI.h"

namespace tb
{
    EditorRenderer* EditorRenderer::_instance = nullptr;

    void EditorRenderer::Create()
    {
        _instance = new EditorRenderer;
    }

    void EditorRenderer::Initialize()
    {
        _renderAPI = static_cast<D3D12RenderAPI*>(RenderAPI::Get());
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