#include "Input.h"

namespace tb
{
    bool Input::IsMouseButtonDown(MouseButton button)
    {
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
        data._bHandled = false;
    }

    void Input::ClearReleasedInput()
    {
        for (auto& [btn, btnData] : _mouseData)
        {
            if (btnData._bHandled && btnData._state == KeyState::Released)
            {
                UpdateButtonState(btn, KeyState::None);
            }
        }
    }
} // namespace tb
