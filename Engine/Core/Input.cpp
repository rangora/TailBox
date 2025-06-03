#include "Input.h"
#include "spdlog/spdlog.h"

namespace tb
{
    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        if (_mouseData[button]._button == button)
        {
            return _mouseData[button]._state == KeyState::Pressed;
        }

        return false;
    }

    bool Input::IsMouseButtonHeld(MouseButton button)
    {
        if (_mouseData[button]._button == button)
        {
            return _mouseData[button]._state == KeyState::Held;
        }

        return false;
    }

    bool Input::IsMouseButtonDown(MouseButton button)
    {
        if (_mouseData[button]._button == button)
        {
            return _mouseData[button]._state == KeyState::Pressed || _mouseData[button]._state == KeyState::Held;
        }

        return false;
    }

    void Input::TransitionPressedButtons()
    {
        for (auto& [btn, btnData] : _mouseData)
        {
            if (btnData._state == KeyState::Pressed)
            {
                if (btnData._bHandled)
                {
                    UpdateButtonState(btn, KeyState::Held);
                }
            }
        }
    }

    void Input::UpdateButtonState(MouseButton button, KeyState state)
    {
        auto& data = _mouseData[button];
        data._button = button;
        data._oldState = data._state;
        data._state = state;
        data._bHandled = true;
    }

    void Input::ClearReleasedInput()
    {
        for (auto& [btn, btnData] : _mouseData)
        {
            if (btnData._bHandled && btnData._state == KeyState::Released)
            {
                spdlog::info("button released");
                UpdateButtonState(btn, KeyState::None);
            }
        }
    }
} // namespace tb
