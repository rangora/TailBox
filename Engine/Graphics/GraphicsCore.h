#include "Core.h"
#include "CommandContext.h"
#include "RawResourceStorage.h"
#include "DX12Device.h"
#include "Renderer.h"
#include "D3D12RenderAPI.h"

namespace tb
{
    extern CommandContext g_commandContext;
    extern DX12Device g_dx12Device;
    extern Renderer g_renderer;
    extern RawResourceStorage g_graphicsResources;
    extern D3D12RenderAPI* g_renderAPI;
} // namespace tb