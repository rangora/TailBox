#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace tb
{
    struct alignas(256) Transform
    {
        SimpleMath::Vector3 _pos = {0.f, 0.f, 0.f};
        SimpleMath::Vector3 _rot = {0.f, 0.f, 0.f};
        SimpleMath::Vector3 _scale = {1.f, 1.f, 1.f};
    };
}; // namespace tb