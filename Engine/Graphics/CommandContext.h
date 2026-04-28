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

        void SetCommandList(ID3D12GraphicsCommandList* cmdList) { _commandList = cmdList; }
        ID3D12GraphicsCommandList* GetCommandList() const { return _commandList; }

        void SetRootSignature(const std::string& name);

        std::vector<std::string> _reservedRootSignature;

        ID3D12GraphicsCommandList* _commandList = nullptr;

        std::unique_ptr<class DescriptorPool> _descriptorPool = nullptr;
        std::unique_ptr<class SolidDescriptorPool> _solidDescriptorPool = nullptr;
        std::unique_ptr<class GuiDescriptorPool> _guiDescriptorPool = nullptr;
        std::unique_ptr<class ConstantBufferPool> _constantBufferPool = nullptr;
    private:
    };
} // namespace tb