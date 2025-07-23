#include "CubeActor.h"
#include "Scene/Scene.h"
#include "Scene/StaticMeshComponent.h"

namespace tb
{
    CubeActor::CubeActor(Scene* scene) : Actor(scene)
    {
        StaticMeshComponent* staticMeshComp = new StaticMeshComponent(this);
        staticMeshComp->SetStaticMesh("Cube");
        _registeredCompoent.push_back(staticMeshComp);
    }

    CubeActor::~CubeActor()
    {
    }
} // namespace tb