#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace tb
{
    struct Transform
    {
        DirectX::XMFLOAT3 _pos   = {};
        DirectX::XMFLOAT3 _rot   = {};
        DirectX::XMFLOAT3 _scale = {};
    };
}; // namespace tb