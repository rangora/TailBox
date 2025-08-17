#include "CubeActor.h"
#include "Scene/Scene.h"
#include "Scene/StaticMeshComponent.h"

namespace tb
{
    CubeActor::CubeActor(Scene* scene) : Actor(scene)
    {
        StaticMeshComponent* staticMeshComp = new StaticMeshComponent(this);
        RegisterComponent(staticMeshComp);

        staticMeshComp->SetStaticMesh("Cube");
    }

    CubeActor::~CubeActor()
    {
    }
} // namespace tb