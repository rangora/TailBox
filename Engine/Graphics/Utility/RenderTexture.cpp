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

        _clearColor[0] = 1.0f; // R
        _clearColor[1] = 0.0f; // G
        _clearColor[2] = 0.0f; // B
        _clearColor[3] = 1.0f; // A

        _state = D3D12_RESOURCE_STATE_RENDER_TARGET;

        CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC desc =
            CD3DX12_RESOURCE_DESC::Tex2D(_format, static_cast<UINT64>(_width), static_cast<UINT>(_height), 1, 1, 1, 0,
                                         D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        D3D12_CLEAR_VALUE clearValue = {_format, {}};
        memcpy(clearValue.Color, _clearColor, sizeof(clearValue.Color));

        g_dx12Device.GetDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &desc,
                                                          _state, &clearValue, IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf()));


        g_dx12Device.GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
        g_dx12Device.GetDevice()->CreateShaderResourceView(_resource.Get(), nullptr, _srvHandle);

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = 1;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            if (g_dx12Device.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap)) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create dsHeap.");
                return;
            }

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = DXGI_FORMAT_D32_FLOAT;
            clearValue.DepthStencil.Depth = 1.f;
            clearValue.DepthStencil.Stencil = 0.f;

            UINT _rw = 800;
            UINT _rh = 600;

            D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_D32_FLOAT, _rw, _rh, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            g_dx12Device.GetDevice()->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                             D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&_dsBuffer));
            if (g_dx12Device.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap)) != S_OK)
            {
                spdlog::error("[FATAL] Failed to create dsHeap after commit.");
                return;
            }
            _dsHeap->SetName(L"depth/stencil resource heap2");
            g_dx12Device.GetDevice()->CreateDepthStencilView(_dsBuffer.Get(), &dsvDesc,
                                                             _dsHeap->GetCPUDescriptorHandleForHeapStart());
        }
    }

    void RenderTexture::Clear(ID3D12GraphicsCommandList* commandList)
    {
        _viewport.Width = 800;  // RTV width
        _viewport.Height = 600; // RTV height
        _rect.right = 800;
        _rect.bottom = 600;

        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_rect);

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());
        commandList->ClearRenderTargetView(_rtvHandle, _clearColor, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(1, &_rtvHandle, FALSE, &dsvHandle);
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