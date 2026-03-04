#include "RenderPassManager.h"
#include "RenderPassForwardRendering.h"
#include "RenderPassUIRendering.h"
#include "RenderPass.h"
#include "GraphicsCore.h"

namespace tb
{
    RenderPassManager* RenderPassManager::_instance = nullptr;

    RenderPassManager::RenderPassManager()
    {
        _renderPasses.resize(RenderpassType::RENDER_PASS_MAX);
        _renderPasses[RenderpassType::RENDER_PASS_FORWARD_RENDERING] = new RenderPassForwardRendering(g_dx12Device.GetDevice());
        _renderPasses[RenderpassType::RENDER_PASS_UI_RENDERING] = new RenderPassUIRendering(g_dx12Device.GetDevice());
    }

    RenderPassManager::~RenderPassManager()
    {
    }

    void RenderPassManager::Create()
    {
        _instance = new RenderPassManager();
    }

    RenderPassManager* RenderPassManager::Get()
    {
        return _instance;
    }

    void RenderPassManager::SetupRenderPass(RenderPiplineType type)
    {
        _currentPasses.clear();

        if (type == RenderPiplineType::Forward)
        {
            _currentPasses.push_back(_renderPasses[RenderpassType::RENDER_PASS_FORWARD_RENDERING]);
            _currentPasses.push_back(_renderPasses[RenderpassType::RENDER_PASS_UI_RENDERING]);
        }
    }

    void RenderPassManager::Render(const D3D12View& view)
{
        for (RenderPass* renderPass : _currentPasses)
        {
            renderPass->Render(view);
        }
    }

} // namespace tb