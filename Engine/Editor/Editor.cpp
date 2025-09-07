#include "Editor.h"
#include "Window/Window.h"

namespace tb
{
    Editor::~Editor()
    {
        if (_window)
        {
            delete _window;
        }
    }

    void Editor::ShutDown()
    {
        if (_window)
        {
            _window->ShutdownImGuiContext();
            _window->ShutdownWindow();
        }
    }

    void Editor::Update(float delta)
    {
        _window->Update();
    }

    void Editor::CreateWinWindow(const WindowContext& _windowContext)
    {
        _window = new Window({"TailBox", VIEWPORT_WIDTH, VIEWPORT_HEIGHT});
    }

    void Editor::BindDevice(DX12Device* device)
    {
        _window->Initialize(device);
    }

    void Editor::CreateDefaultLayout()
    {
    }
} // namespace tb