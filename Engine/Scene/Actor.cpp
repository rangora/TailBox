#include "Actor.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/PipelineStateHandler.h"
#include "Graphics/TextureResource.h"
#include "Graphics/GraphicsCore.h"
#include "Scene/Scene.h"
#include "Scene/StaticMeshComponent.h"

namespace tb
{
    Actor::Actor(Scene* scene)
    {
        StaticMeshComponent* staticMeshComp = new StaticMeshComponent(this);
        staticMeshComp->SetStaticMesh("Cube");
        _registeredCompoent.push_back(staticMeshComp);

        scene->RegisterActor(this);
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