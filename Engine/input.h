#pragma once
#include <directxtk12/Keyboard.h>
#include <directxtk12/Mouse.h>


class Input
{
    friend class engine::Engine;

    DirectX::Keyboard::State m_KeyboardState;
    DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
    std::unique_ptr<DirectX::Keyboard> m_Keyboard;
    std::unique_ptr<DirectX::Mouse> m_Mouse;
    DirectX::Mouse::State m_MouseState;
    DirectX::Mouse::ButtonStateTracker m_MouseTracker;

    Input();
    void Init();
    void Update();

    [[nodiscard]] static Input *m_instance_;

public:
    static Input *Get();
    DirectX::Keyboard *Keyboard() const;

    [[nodiscard]] static bool IsKeyDown(DirectX::Keyboard::Keys key);
    [[nodiscard]] static bool IsKeyPressed(DirectX::Keyboard::Keys key);
    [[nodiscard]] static bool IsKeyReleased(DirectX::Keyboard::Keys key);

    [[nodiscard]] static bool IsLeftButtonDown();
    [[nodiscard]] static bool IsLeftButtonPressed();
    [[nodiscard]] static bool IsRightButtonDown();
    [[nodiscard]] static bool IsRightButtonPressed();
    [[nodiscard]] static Vector2 MousePos();
};