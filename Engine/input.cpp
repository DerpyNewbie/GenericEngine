#include "pch.h"
#include "input.h"
#include "application.h"

namespace engine
{
Input::Input()
{
    m_keyboard_ = std::make_unique<DirectX::Keyboard>();
    m_mouse_ = std::make_unique<DirectX::Mouse>();
}

void Input::Init()
{
    m_mouse_->SetWindow(Application::Instance()->GetWindowHandle());
    m_mouse_->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}

void Input::ProcessMessage(const UINT msg, const WPARAM w_param, const LPARAM l_param)
{
    auto instance = Instance();
    instance->m_keyboard_->ProcessMessage(msg, w_param, l_param);
    instance->m_mouse_->ProcessMessage(msg, w_param, l_param);
}

void Input::Update()
{
    m_keyboard_state_ = m_keyboard_->GetState();
    m_keyboard_tracker_.Update(m_keyboard_state_);

    m_mouse_state_ = m_mouse_->GetState();
    m_mouse_tracker_.Update(m_mouse_state_);

    if (m_mouse_state_.positionMode == DirectX::Mouse::MODE_ABSOLUTE)
    {
        const auto mouse_pos = MousePosition();
        m_mouse_delta_ = mouse_pos - m_mouse_position_;
        m_mouse_position_ = mouse_pos;
    }
    else
    {
        m_mouse_delta_ = MousePosition();
    }
}

std::shared_ptr<Input> Input::Instance()
{
    static auto instance = std::make_shared<Input>();
    return instance;
}

bool Input::GetKey(DirectX::Keyboard::Keys key)
{
    return Instance()->m_keyboard_state_.IsKeyDown(key);
}

bool Input::GetKeyDown(DirectX::Keyboard::Keys key)
{
    return Instance()->m_keyboard_tracker_.IsKeyPressed(key);
}

bool Input::GetKeyUp(DirectX::Keyboard::Keys key)
{
    return Instance()->m_keyboard_tracker_.IsKeyReleased(key);
}

bool Input::GetMouseLeft()
{
    return Instance()->m_mouse_state_.leftButton;
}

bool Input::GetMouseLeftDown()
{
    return Instance()->m_mouse_tracker_.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::GetMouseLeftUp()
{
    return Instance()->m_mouse_tracker_.leftButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
}

bool Input::GetMouseRight()
{
    return Instance()->m_mouse_state_.rightButton;
}

bool Input::GetMouseRightDown()
{
    return Instance()->m_mouse_tracker_.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::GetMouseRightUp()
{
    return Instance()->m_mouse_tracker_.rightButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
}

Vector2 Input::MousePosition()
{
    return {static_cast<float>(Instance()->m_mouse_state_.x), static_cast<float>(Instance()->m_mouse_state_.y)};
}

Vector2 Input::MouseDelta()
{
    return Instance()->m_mouse_delta_;
}

kMouseMode Input::MouseMode()
{
    return Instance()->m_mouse_mode_;
}

void Input::SetMouseMode(const kMouseMode mode)
{
    Instance()->m_mouse_mode_ = mode;

    switch (mode)
    {
    case kMouseMode::kNormal:
        Instance()->m_mouse_->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
        return;
    case kMouseMode::kLocked:
        Instance()->m_mouse_->SetMode(DirectX::Mouse::MODE_RELATIVE);
        return;
    }
}

void Input::SetCursorVisible(const bool is_visible)
{
    Instance()->m_mouse_->SetVisible(is_visible);
}
}