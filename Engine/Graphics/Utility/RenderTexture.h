#pragma once

#include "Core.h"

namespace tb
{
    class RenderTexture
    {
    public:
        explicit RenderTexture(DXGI_FORMAT format) noexcept;

        RenderTexture(RenderTexture const&) = delete;
        RenderTexture& operator=(RenderTexture const&) = delete;

        void Release();

        void SetSrvHandle(D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
        void SetRtvHandle(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
        void CreateResource(int32 width, int32 height);
        void Clear(ID3D12GraphicsCommandList* commandList);

        bool TransitionTo(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);

    private:
        ComPtr<ID3D12Resource> _resource = nullptr;
        ComPtr<ID3D12Resource> _dsBuffer = nullptr;
        ComPtr<ID3D12DescriptorHeap> _dsHeap = nullptr;
        D3D12_RESOURCE_STATES _state;
        D3D12_CPU_DESCRIPTOR_HANDLE _srvHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle;

        D3D12_VIEWPORT _viewport = {};
        D3D12_RECT _rect = {};

        DXGI_FORMAT _format;
        int32 _width = 100;
        int32 _height = 100;
        float _clearColor[4] = {};
    };
}