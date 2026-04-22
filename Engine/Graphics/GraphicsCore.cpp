#include "GraphicsCore.h"

namespace tb
{
    CommandContext g_commandContext;
    Renderer g_renderer;
    RawResourceStorage g_graphicsResources;
    D3D12RenderAPI* g_renderAPI = nullptr;
}
