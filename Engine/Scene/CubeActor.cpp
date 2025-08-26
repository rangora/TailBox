#include "CubeActor.h"
#include "Scene/Scene.h"
#include "Scene/StaticMeshComponent.h"

namespace tb
{
    CubeActor::CubeActor(Scene* scene) : Actor(scene)
    {
        _staticMeshComponent = new StaticMeshComponent(this);
        RegisterComponent(_staticMeshComponent);

        _staticMeshComponent->SetStaticMesh("Cube");
    }

    CubeActor::~CubeActor()
    {
    }
} // namespace tb