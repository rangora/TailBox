#pragma once

#include "Core.h"
#include <vector>

namespace tb
{
    class CommandContext
    {
    public:
        CommandContext();
        ~CommandContext();

        void SetRootSignature(const std::string& name);

        std::vector<std::string> _reservedRootSignature;
    };
} // namespace tb