#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace tb
{
    struct alignas(256) Transform
    {
        SimpleMath::Vector3 _pos = {};
        SimpleMath::Vector3 _rot = {};
        SimpleMath::Vector3 _scale = {};
    };
}; // namespace tb