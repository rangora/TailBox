#pragma once

#include "Core.h"

namespace tb
{
    class Camera
    {
    public:
        XMVECTOR _posVector;
        XMVECTOR _rotVector;
        Vector _pos = {0.f, 0.f, 0.f};
        Vector _rot = {0.f, 0.f, 0.f};

        XMMATRIX _viewMtx;
        XMMATRIX _projMtx;

        XMVECTOR _forward;
        XMVECTOR _left;
        XMVECTOR _right;
        XMVECTOR _backward;

        void UpdateViewMatrix(); // may be private
        void SetProjection(float fov, float aspectRatio, float nearZ, float farZ);

        void SetDirection(Vector direction);
        void SetPosition(const Vector& pos);
        void SetPosition(float x, float y, float z);
        void SetRotation(const Vector& rot);
        void SetRotation(float x, float y, float z);
        void AddPosition(const Vector& delta);
        void AddPosition(float x, float y, float z);
        void AddRotation(const Vector& delta);
        void AddRotation(float x, float y, float z);

        void OnUpdate(float in_delta); // will be deprecated
    };
};