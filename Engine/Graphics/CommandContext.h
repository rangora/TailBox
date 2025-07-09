#pragma once

#include "Core.h"
#include "DX12Device.h"

namespace tb
{
    class CommandContext
    {
    public:
        friend DX12Device;

        void SetRootSignature();
    };
} // namespace tb