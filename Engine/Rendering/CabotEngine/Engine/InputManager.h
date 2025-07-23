#pragma once
#include "pch.h"
#include <directxtk12/Keyboard.h>
#include <directxtk12/Mouse.h>


class InputManager
{
public:
    InputManager();

    void Initialize();
    void Update();

    [[nodiscard]] bool IsKeyDown(DirectX::Keyboard::Keys key) const;
    bool IsKeyPressed(DirectX::Keyboard::Keys key) const;
    bool IsKeyReleased(DirectX::Keyboard::Keys key) const;

    bool IsLeftButtonDown() const;
    bool IsLeftButtonPressed() const;
    bool IsRightButtonDown() const;
    bool IsRightButtonPressed() const;

    Vector2 MousePos();

    std::unique_ptr<DirectX::Keyboard> m_Keyboard;

private:
    DirectX::Keyboard::State m_KeyboardState;
    DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;

    std::unique_ptr<DirectX::Mouse> m_Mouse;
    DirectX::Mouse::State m_MouseState;
    DirectX::Mouse::ButtonStateTracker m_MouseTracker;
};

extern InputManager g_Input;