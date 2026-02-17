#pragma once
#include <directxtk12/Keyboard.h>
#include <directxtk12/Mouse.h>

namespace engine
{
enum class kMouseMode
{
    kNormal,
    kLocked
};

class Input
{
    friend class Application;
    friend class Engine;

    DirectX::Keyboard::State m_keyboard_state_;
    DirectX::Keyboard::KeyboardStateTracker m_keyboard_tracker_;
    std::unique_ptr<DirectX::Keyboard> m_keyboard_;
    std::unique_ptr<DirectX::Mouse> m_mouse_;
    DirectX::Mouse::State m_mouse_state_;
    DirectX::Mouse::ButtonStateTracker m_mouse_tracker_;
    kMouseMode m_mouse_mode_;
    Vector2 m_mouse_position_;
    Vector2 m_mouse_delta_;

    void Init() const;
    void Update();

public:
    static std::shared_ptr<Input> Instance();
    static void ProcessMessage(UINT msg, WPARAM w_param, LPARAM l_param);

    Input();

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
    [[nodiscard]] static Vector2 MouseDelta();
    [[nodiscard]] static kMouseMode MouseMode();

    static void SetMouseMode(kMouseMode mode);
    static void SetCursorVisible(bool is_visible);
};
}