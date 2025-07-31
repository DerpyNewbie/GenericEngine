#include "pch.h"
#include "Input.h"
#include "application.h"

namespace engine
{
Input *Input::m_instance_;

Input::Input()
{
    m_keyboard_ = std::make_unique<DirectX::Keyboard>();
    m_mouse_ = std::make_unique<DirectX::Mouse>();
}

void Input::Init()
{
    m_mouse_->SetWindow(Application::GetWindowHandle());
    m_mouse_->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}

void Input::Update()
{
    m_keyboard_state_ = m_keyboard_->GetState();
    m_keyboard_tracker_.Update(m_keyboard_state_);

    m_mouse_state_ = m_mouse_->GetState();
    m_mouse_tracker_.Update(m_mouse_state_);
}

Input *Input::Get()
{
    if (m_instance_ == nullptr)
    {
        m_instance_ = new Input;
    }
    return m_instance_;
}

DirectX::Keyboard *Input::Keyboard() const
{
    return m_keyboard_.get();
}

bool Input::GetKey(DirectX::Keyboard::Keys key)
{
    return m_instance_->m_keyboard_state_.IsKeyDown(key);
}

bool Input::GetKeyDown(DirectX::Keyboard::Keys key)
{
    return m_instance_->m_keyboard_tracker_.IsKeyPressed(key);
}

bool Input::GetKeyUp(DirectX::Keyboard::Keys key)
{
    return m_instance_->m_keyboard_tracker_.IsKeyReleased(key);
}

bool Input::GetMouseLeft()
{
    return m_instance_->m_mouse_state_.leftButton;
}

bool Input::GetMouseLeftDown()
{
    return m_instance_->m_mouse_tracker_.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::GetMouseLeftUp()
{
    return m_instance_->m_mouse_tracker_.leftButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
}

bool Input::GetMouseRight()
{
    return m_instance_->m_mouse_state_.rightButton;
}

bool Input::GetMouseRightDown()
{
    return m_instance_->m_mouse_tracker_.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::GetMouseRightUp()
{
    return m_instance_->m_mouse_tracker_.rightButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
}

Vector2 Input::MousePosition()
{
    return {static_cast<float>(m_instance_->m_mouse_state_.x), static_cast<float>(m_instance_->m_mouse_state_.y)};
}
}