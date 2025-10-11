#include "RenderTexture.h"
#include "Graphics/GraphicsCore.h"

namespace tb
{
    RenderTexture::RenderTexture(DXGI_FORMAT format) noexcept
        : _state(D3D12_RESOURCE_STATE_COMMON),
        _srvHandle{},
        _rtvHandle{},
        _format(format),
        _width(100), _height(100)
    {
    }

    void RenderTexture::Release()
    {
        if (_resource)
        {
            _resource.Reset();
        }

        _srvHandle.ptr = 0;
        _rtvHandle.ptr = 0;
    }

    void RenderTexture::SetSrvHandle(D3D12_CPU_DESCRIPTOR_HANDLE srvHandle)
    {
        _srvHandle = srvHandle;
    }

    void RenderTexture::SetRtvHandle(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle)
    {
        _rtvHandle = rtvHandle;
    }

    void RenderTexture::CreateResource(int32 width, int32 height)
    {
        _width = width;
        _height = height;

        CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC desc =
            CD3DX12_RESOURCE_DESC::Tex2D(_format, static_cast<UINT64>(_width), static_cast<UINT>(_height), 1, 1, 1, 0,
                                         D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        _clearColor[0] = 1.0f; // R
        _clearColor[1] = 0.0f; // G
        _clearColor[2] = 0.0f; // B
        _clearColor[3] = 1.0f; // A
        D3D12_CLEAR_VALUE clearValue = {_format, {}};
        memcpy(clearValue.Color, _clearColor, sizeof(clearValue.Color));

        _state = D3D12_RESOURCE_STATE_RENDER_TARGET;

        g_dx12Device.GetDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &desc,
                                                          _state, &clearValue, IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf()));


        g_dx12Device.GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
        g_dx12Device.GetDevice()->CreateShaderResourceView(_resource.Get(), nullptr, _srvHandle);
    }

    void RenderTexture::Clear(ID3D12GraphicsCommandList* commandList)
    {
        commandList->ClearRenderTargetView(_rtvHandle, _clearColor, 0, nullptr);
    }

    bool RenderTexture::TransitionTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState)
    {
        if (_state == newState)
        {
            return false;
        }

        D3D12_RESOURCE_BARRIER desc = {};
        desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        desc.Transition.pResource = _resource.Get();
        desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        desc.Transition.StateBefore = _state;
        desc.Transition.StateAfter = newState;
        commandList->ResourceBarrier(1, &desc);

        _state = newState;

        return true;
    }

} // namespace tb