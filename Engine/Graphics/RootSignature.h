#pragma once

#include "Core.h"

namespace tb
{
    class RootSignature
    {
    public:
        void CreateRootSignature();

        ComPtr<ID3D12RootSignature> _rootSignature = nullptr;
    };
}