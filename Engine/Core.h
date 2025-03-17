#pragma once

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

#include <string>
#include <memory>

#include "dxgi1_4.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <wrl.h>
#include <spdlog/spdlog.h>
#include "SimpleMath.h"
#include <d3dx12.h>

using namespace DirectX;
using namespace Microsoft::WRL;

using Vector2D = XMFLOAT2;
using Vector   = XMFLOAT3;
using Vector4D = XMFLOAT4;
using Matrix   = XMMATRIX;

// ¾Ö¸Å..
struct Vertex
{
    Vector _pos;
    Vector4D _color;
};

struct Transform
{
    Vector4D _offset;
};