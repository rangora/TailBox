#pragma once

#include <map>

namespace tb
{
    enum class KeyButton : int
    {
        W,
        S,
        A,
        D
    };

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
        bool _bHandled = true; // ??
    };

    struct KeyData
    {
        KeyButton _button;
        KeyState _state = KeyState::None;
        KeyState _oldState = KeyState::None;
        bool _bHandled = true; // ??
    };

    class Input
    {
    public:
        static bool IsMouseButtonPressed(MouseButton button);
        static bool IsMouseButtonHeld(MouseButton button);
        static bool IsMouseButtonDown(MouseButton button);
        static void UpdateMouseButtonState(MouseButton button, KeyState state);

        static bool IsKeyButtonPressed(KeyButton button);
        static bool IsKeyButtonHeld(KeyButton button);
        static bool IsKeyButtonDown(KeyButton button);
        static void UpdateKeyButtonState(KeyButton button, KeyState state);

        static void TransitionPressedButtons();
        static void ClearReleasedInput();



    private:
        inline static std::map<MouseButton, ButtonData> _mouseData;
        inline static std::map<KeyButton, KeyData> _keyData;
    };
};