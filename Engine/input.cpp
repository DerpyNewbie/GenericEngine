#include "pch.h"
#include "Input.h"
#include "application.h"

Input *Input::m_instance_;

Input::Input()
{
    m_Keyboard = std::make_unique<DirectX::Keyboard>();
    m_Mouse = std::make_unique<DirectX::Mouse>();
}

void Input::Init()
{
    m_Mouse->SetWindow(Application::GetWindowHandle());
    m_Mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}

void Input::Update()
{
    m_KeyboardState = m_Keyboard->GetState();
    m_KeyboardTracker.Update(m_KeyboardState);

    m_MouseState = m_Mouse->GetState();
    m_MouseTracker.Update(m_MouseState);
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
    return m_Keyboard.get();
}

bool Input::IsKeyDown(DirectX::Keyboard::Keys key)
{
    return m_instance_->m_KeyboardState.IsKeyDown(key);
}

bool Input::IsKeyPressed(DirectX::Keyboard::Keys key)
{
    return m_instance_->m_KeyboardTracker.IsKeyPressed(key);
}

bool Input::IsKeyReleased(DirectX::Keyboard::Keys key)
{
    return m_instance_->m_KeyboardTracker.IsKeyReleased(key);
}

bool Input::IsLeftButtonDown()
{
    return m_instance_->m_MouseState.leftButton;
}

bool Input::IsLeftButtonPressed()
{
    return m_instance_->m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::IsRightButtonDown()
{
    return m_instance_->m_MouseState.rightButton;
}

bool Input::IsRightButtonPressed()
{
    return m_instance_->m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

Vector2 Input::MousePos()
{
    return {static_cast<float>(m_instance_->m_MouseState.x), static_cast<float>(m_instance_->m_MouseState.y)};
}