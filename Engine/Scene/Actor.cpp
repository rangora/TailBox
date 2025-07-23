#include "Actor.h"
#include "SceneComponent.h"
#include "Scene/Scene.h"

namespace tb
{
    Actor::Actor(Scene* scene) : _scene(scene)
    {
        _scene->RegisterActor(this);
    }

    Actor::~Actor()
    {
        for (auto it : _registeredCompoent)
        {
            delete it;
        }
    }

    void Actor::SetLocation(const Vector& location)
    {
        _transform._pos = location;
    }

    void Actor::Render(const XMMATRIX& vpMtx)
    {
        for (auto it : _registeredCompoent)
        {
            it->Render(vpMtx);
        }
    }

    void Actor::Clear()
    {
        for (auto it : _registeredCompoent)
        {
            it->Clear();
        }
    }
} // namespace tb