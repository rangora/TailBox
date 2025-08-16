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

        void Initialize();
        void Reset();

        void SetRootSignature(const std::string& name);

        std::vector<std::string> _reservedRootSignature;

    private:
        std::unique_ptr<class DescriptorPool> _descriptorPool = nullptr;
        std::unique_ptr<class ConstantBufferPool> _constantBufferPool = nullptr;    };
} // namespace tb