#pragma once

#include "Core.h"

namespace tb
{
    struct alignas(256) MeshConstants
    {
        XMFLOAT4X4 _worldMtx;
    };

    struct alignas(256) GlobalConstants
    {
        XMFLOAT4X4 _viewProj;
        Vector3 _cameraPosition;
        float _time;
    };

    struct alignas(256) MaterialConstants
    {
        Vector4 _diffuse;
        Vector4 _specular;
        Vector4 _ambient;
        Vector4 _emissive;
    };

    struct alignas(256) TempConstants
    {
        XMFLOAT4X4 _worldMtx;

        XMFLOAT4X4 _viewProj;
        Vector3 _cameraPosition;
        float _time;

        Vector4 _diffuse;
        Vector4 _specular;
        Vector4 _ambient;
        Vector4 _emissive;
    };

    struct alignas(256) BaseConstants
    {
        MeshConstants _mesh;
        GlobalConstants _global;
        MaterialConstants _material;
    };
}