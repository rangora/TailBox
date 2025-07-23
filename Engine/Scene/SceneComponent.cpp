#include "SceneComponent.h"
#include "Actor.h"
#include "Graphics/GpuBuffer.h"

namespace tb
{
    SceneComponent::SceneComponent(Actor* ownerActor) : _ownerActor(ownerActor)
    {
    }

    SceneComponent::~SceneComponent() = default;

    void SceneComponent::SetLocation(const Vector3& location)
    {
        _transform._pos = location;
    }

    void SceneComponent::Render(const XMMATRIX& vpMtx)
    {
    }

    void SceneComponent::Clear()
    {
    }
}