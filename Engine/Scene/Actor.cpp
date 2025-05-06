#include "Actor.h"
#include "SceneProxy.h"

namespace tb
{
    Actor::Actor()
    {
        _sceneProxy = std::make_unique<SceneProxy>();
    }

    Actor::~Actor()
    {
        if (_sceneProxy)
        {
            _sceneProxy.release();
        }
    }

    void Actor::Tick(float delta)
    {
        if (_sceneProxy)
        {
            _sceneProxy->Tick(delta);
        }
    }
} // namespace tb
