#include "CommandContext.h"
#include "GraphicsCore.h"
#include "RootSignature.h"
#include "MemoryAllocator.h"
#include "ShaderResource.h"

namespace tb
{
    CommandContext::CommandContext() {}
    CommandContext::~CommandContext()
    {
        _descriptorPool.release();
        _constantBufferPool.release();
        _solidDescriptorPool.release();
    }

    void CommandContext::Initialize()
    {
        _descriptorPool = std::make_unique<DescriptorPool>();
        _descriptorPool->Initialize(256);

        _solidDescriptorPool = std::make_unique<SolidDescriptorPool>();
        _solidDescriptorPool->Initialize(256, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

        _constantBufferPool = std::make_unique<class ConstantBufferPool>();
        _constantBufferPool->Initialize(sizeof(TempConstants), 256);

        _reservedRootSignature.push_back("Default"); // TEMP
    }

    void CommandContext::Reset()
    {
        _constantBufferPool->Reset();
        _descriptorPool->Reset();
    }

    void CommandContext::Release()
    {
        if (_descriptorPool)
        {
            _descriptorPool.reset();
        }
        if (_constantBufferPool)
        {
            _constantBufferPool.reset();
        }
        if (_solidDescriptorPool)
        {
            _solidDescriptorPool.reset();
        }
    }

    void CommandContext::SetRootSignature(const std::string& name)
    {
        if (RootSignature* rs = g_renderer.GetRootSignature(name))
        {
            g_dx12Device._commandList->SetGraphicsRootSignature(rs->_rootSignature.Get());
        }
    }
} // namespace tb