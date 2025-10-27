#pragma once

#include "Core.h"
#include "DirectX12/D3D12Struct.h"

namespace tb
{
    class Camera;

    class RenderPass
    {
    public:
        RenderPass() = delete;
        RenderPass(ID3D12Device* device) : _device(device) {}
        ~RenderPass() {};

        virtual void Render(const D3D12View& view) = 0;

    protected:
        ComPtr<ID3D12Device> _device = nullptr;
    };
}