#pragma once
#include <directxtk12/Keyboard.h>
#include <directxtk12/Mouse.h>

namespace engine
{
class Input
{
    friend class Engine;

    DirectX::Keyboard::State m_keyboard_state_;
    DirectX::Keyboard::KeyboardStateTracker m_keyboard_tracker_;
    std::unique_ptr<DirectX::Keyboard> m_keyboard_;
    std::unique_ptr<DirectX::Mouse> m_mouse_;
    DirectX::Mouse::State m_mouse_state_;
    DirectX::Mouse::ButtonStateTracker m_mouse_tracker_;

    Input();
    void Init();
    void Update();

    [[nodiscard]] static Input *m_instance_;

public:
    static Input *Get();
    DirectX::Keyboard *Keyboard() const;

    [[nodiscard]] static bool GetKey(DirectX::Keyboard::Keys key);
    [[nodiscard]] static bool GetKeyDown(DirectX::Keyboard::Keys key);
    [[nodiscard]] static bool GetKeyUp(DirectX::Keyboard::Keys key);

    [[nodiscard]] static bool GetMouseLeft();
    [[nodiscard]] static bool GetMouseLeftDown();
    [[nodiscard]] static bool GetMouseLeftUp();
    [[nodiscard]] static bool GetMouseRight();
    [[nodiscard]] static bool GetMouseRightDown();
    [[nodiscard]] static bool GetMouseRightUp();
    [[nodiscard]] static Vector2 MousePosition();
};
}