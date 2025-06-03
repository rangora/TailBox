#pragma once

#include <map>

namespace tb
{
    enum class MouseButton : int
    {
        Left,
        Right,
        Middle,
    };

    enum class KeyState : int
    {
        None,
        Pressed,
        Held,
        Released,
    };

    struct ButtonData
    {
        MouseButton _button;
        KeyState _state = KeyState::None;
        KeyState _oldState = KeyState::None;
        bool _bHandled = true;
    };

    class Input
    {
    public:
        static bool IsMouseButtonPressed(MouseButton button);
        static bool IsMouseButtonHeld(MouseButton button);
        static bool IsMouseButtonDown(MouseButton button);
        static void TransitionPressedButtons();
        static void UpdateButtonState(MouseButton button, KeyState state);
        static void ClearReleasedInput();

    private:
        inline static std::map<MouseButton, ButtonData> _mouseData;
    };
};