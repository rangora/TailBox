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

// Core
#include "Core/Transform.h"

constexpr int32 VIEWPORT_WIDTH = 960;
constexpr int32 VIEWPORT_HEIGHT = 540;

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;


#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)


namespace tb
{
    namespace core
    {
        const std::string projectPath = PROJECT_PATH;
    }
}