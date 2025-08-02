#pragma once

#include "Core.h"

namespace tb
{
    class Actor;
    class GpuBuffer;

    class SceneComponent
    {
    public:
        SceneComponent() = delete;
        SceneComponent(Actor* ownerActor);
        ~SceneComponent();

        void SetLocation(const Vector3& location);

        // proxy functions
        virtual void Render(const XMMATRIX& vpMtx);
        virtual void Clear();

    protected:
        Actor* _ownerActor;
        Transform _transform;
        Matrix _worldMatrix;

    private:
    };
} // namespace tb