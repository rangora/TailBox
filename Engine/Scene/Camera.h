#pragma once

#include "Core.h"

namespace tb
{
    class Camera
    {
    public:
        XMVECTOR _posVector = {};
        XMVECTOR _rotVector = {};
        Vector3 _pos = {0.f, 0.f, 0.f};
        Vector3 _rot = {0.f, 0.f, 0.f};

        XMMATRIX _viewMtx = {};
        XMMATRIX _projMtx = {};

        XMVECTOR _forward = {};
        XMVECTOR _left = {};
        XMVECTOR _right = {};
        XMVECTOR _backward = {};

        void UpdateViewMatrix(); // may be private
        void SetProjection(float fov, float aspectRatio, float nearZ, float farZ);

        void SetDirection(Vector3 direction);
        void SetPosition(const Vector3& pos);
        void SetPosition(float x, float y, float z);
        void SetRotation(const Vector3& rot);
        void SetRotation(float x, float y, float z);
        void AddPosition(const Vector3& delta);
        void AddPosition(float x, float y, float z);
        void AddRotation(const Vector3& delta);
        void AddRotation(float x, float y, float z);

        void OnUpdate(float in_delta); // will be deprecated
    };
};