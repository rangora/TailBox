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
        if (_scene)
        {
            Destroy();
        }
    }

    void Actor::Destroy()
    {
        // scene이 없다 == destroy되었다.
        _scene = nullptr;

        for (auto comp : _registeredCompoent)
        {
            delete comp;
        }
    }

    void Actor::SetLocation(const Vector3& location)
    {
        _transform._pos = location;
    }

    void Actor::SetScale(const Vector3& scale)
    {
        _transform._scale = scale;
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

    void Actor::RegisterComponent(SceneComponent* component)
    {
        _registeredCompoent.push_back(component);
    }
} // namespace tb