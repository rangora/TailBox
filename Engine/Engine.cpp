#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Window/Window.h"
#include <vector>

namespace tb
{
    DX12Device* Engine::_DX12device = nullptr;

    Engine::Engine()
    {
        _instance = this;
    }

    Engine::~Engine()
    {
    }

    DX12Device* Engine::GetDX12Device()
    {
        return _DX12device;
    }

    ID3D12Device* Engine::GetDevice()
    {
        return _DX12device->GetDevice();
    }

    void Engine::Launch()
    {
        _DX12device = new DX12Device;
        _window = new Window({"TailBox", 1000, 600});
        _DX12device->CreateSwapChain(_window->GetWndRef());
        _DX12device->PostSwapChainCreated();
        _window->Initialize();

        TEMP_Initial();

        while (!_bQuit)
        {
            Tick(0.f);
        }

        // Sutting down
        _DX12device->WaitForLastSubmittedFrame();
        _window->ShutdownImGuiContext();
        _DX12device->ReleaseDevice();
        _window->ShutdownWindow();
    }

    void Engine::Tick(const float tick)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                _bQuit = true;
                break;
            }
        }

        if (_bQuit)
        {
            return;
        }

        if (_DX12device->IsScreenLocked())
        {
            ::Sleep(10);
            return;
        }

        _window->Update();
        _DX12device->Update();

        //_window->PostRenderEnd(); // hardcode
    }

    void Engine::TEMP_Initial()
    {
        // TEMP
        std::vector<Vertex> Vertexvec(4);
        Vertexvec[0]._pos = Vector(-0.5f, 0.5f, 0.5f);
        Vertexvec[0]._color = Vector4D(1.f, 0.f, 0.f, 1.f);
        Vertexvec[1]._pos = Vector(0.5f, 0.5f, 0.5f);
        Vertexvec[1]._color = Vector4D(0.f, 1.f, 0.f, 1.f);
        Vertexvec[2]._pos = Vector(0.5f, -0.5f, 0.5f);
        Vertexvec[2]._color = Vector4D(0.f, 0.f, 1.f, 1.f);
        Vertexvec[3]._pos = Vector(-0.5f, -0.5f, 0.5f);
        Vertexvec[3]._color = Vector4D(0.f, 1.f, 0.f, 1.f);

        std::vector<uint32> indexVec;
        {
            indexVec.push_back(0);
            indexVec.push_back(1);
            indexVec.push_back(2);
        }
        {
            indexVec.push_back(0);
            indexVec.push_back(2);
            indexVec.push_back(3);
        }

        _mesh = new Mesh(Vertexvec, indexVec);
        _shader = new Shader(L"E:\\workspace\\TailBox\\Engine\\Resources\\default.hlsli",
                            L"E:\\workspace\\TailBox\\Engine\\Resources\\default.hlsli");
    }
} // namespace tb