#include "CommandContext.h"
#include "GraphicsCore.h"
#include "RootSignature.h"

namespace tb
{
    CommandContext::CommandContext()
    {
        _reservedRootSignature.push_back("Default"); // TEMP
    }

    CommandContext::~CommandContext() = default;

    void CommandContext::SetRootSignature(const std::string& name)
    {
        if (RootSignature* rs = g_renderer.GetRootSignature(name))
        {
            g_dx12Device._commandList->SetGraphicsRootSignature(rs->_rootSignature.Get());
        }
    }
} // namespace tb