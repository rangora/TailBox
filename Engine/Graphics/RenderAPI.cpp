#include "RenderAPI.h"
#include "D3D12RenderAPI.h"

namespace tb
{
    RenderAPI* RenderAPI::_instance = nullptr;

    RenderAPI::RenderAPI()
    {
    }

    RenderAPI::~RenderAPI()
    {
    }

    void RenderAPI::Create()
    {
        // each framework
        _instance = new D3D12RenderAPI();
    }

    RenderAPI* RenderAPI::Get()
    {
        return _instance;
    }

} // namespace tb