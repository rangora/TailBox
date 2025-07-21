#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace tb
{
    struct alignas(256) Transform
    {
        DirectX::XMFLOAT3 _pos   = {};
        DirectX::XMFLOAT3 _rot   = {};
        DirectX::XMFLOAT3 _scale = {};
    };

    struct alignas(256) Matrix
    {
        XMFLOAT4X4 _wvpMat;
    };
}; // namespace tb