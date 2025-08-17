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
        void Release();

        void SetRootSignature(const std::string& name);

        std::vector<std::string> _reservedRootSignature;

        std::unique_ptr<class DescriptorPool> _descriptorPool = nullptr;
        std::unique_ptr<class ConstantBufferPool> _constantBufferPool = nullptr;
    private:
    };
} // namespace tb