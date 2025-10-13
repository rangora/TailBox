#include "RenderPassManager.h"

namespace tb
{
    RenderPassManager* RenderPassManager::_instance = nullptr;

    RenderPassManager::RenderPassManager()
    {
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

} // namespace tb