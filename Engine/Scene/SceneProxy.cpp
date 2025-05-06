#include "SceneProxy.h"
#include "Engine.h"

namespace tb
{
    SceneProxy::SceneProxy()
    {
        _bRenderMarkDirty = true;
    }

    SceneProxy::~SceneProxy()
    {
    }

    void SceneProxy::Render()
    {
    }

    void SceneProxy::Tick(float delta)
    {
        if (_bRenderMarkDirty)
        {

        }
    }
} // namespace tb