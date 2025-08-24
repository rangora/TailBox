#pragma once

#include "Core.h"

namespace tb
{
    class RootSignature
    {
    public:
        RootSignature();
        ~RootSignature();

        void CreateMaterialRootSignature();

        ComPtr<ID3D12RootSignature> _rootSignature = nullptr;
    };
}