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

    void Input::UpdateMouseButtonState(MouseButton button, KeyState state)
    {
        auto& data = _mouseData[button];
        data._button = button;
        data._oldState = data._state;
        data._state = state;
        data._bHandled = true;
    }

    bool Input::IsKeyButtonPressed(KeyButton button)
    {
        if (_keyData[button]._button == button)
        {
            return _keyData[button]._state == KeyState::Pressed;
        }

        return false;
    }

    bool Input::IsKeyButtonHeld(KeyButton button)
    {
        if (_keyData[button]._button == button)
        {
            return _keyData[button]._state == KeyState::Held;
        }

        return false;
    }

    bool Input::IsKeyButtonDown(KeyButton button)
    {
        if (_keyData[button]._button == button)
        {
            return _keyData[button]._state == KeyState::Pressed || _keyData[button]._state == KeyState::Held;
        }

        return false;
    }

    void Input::UpdateKeyButtonState(KeyButton button, KeyState state)
    {
        auto& data = _keyData[button];
        data._button = button;
        data._oldState = data._state;
        data._state = state;
        data._bHandled = true;
    }

    void Input::TransitionPressedButtons()
    {
        for (auto& [btn, btnData] : _mouseData)
        {
            if (btnData._state == KeyState::Pressed)
            {
                if (btnData._bHandled)
                {
                    UpdateMouseButtonState(btn, KeyState::Held);
                }
            }
        }

        for (auto& [btn, btnData] : _keyData)
        {
            if (btnData._state == KeyState::Pressed)
            {
                if (btnData._bHandled)
                {
                    UpdateKeyButtonState(btn, KeyState::Held);
                }
            }
        }
    }

    void Input::ClearReleasedInput()
    {
        for (auto& [btn, btnData] : _mouseData)
        {
            if (btnData._bHandled && btnData._state == KeyState::Released)
            {
                UpdateMouseButtonState(btn, KeyState::None);
            }
        }

        for (auto& [btn, btnData] : _keyData)
        {
            if (btnData._bHandled && btnData._state == KeyState::Released)
            {
                UpdateKeyButtonState(btn, KeyState::None);
            }
        }
    }
} // namespace tb
