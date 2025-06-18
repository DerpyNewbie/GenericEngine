#include "pch.h"

#include "Input.h"

#include "application.h"

std::unique_ptr<Input> g_pInput;

Input::Input()
{
    m_Keyboard = std::make_unique<DirectX::Keyboard>();
    m_Mouse = std::make_unique<DirectX::Mouse>();
}

void Input::Initialize()
{
    m_Mouse->SetWindow(g_app->GetWindowHandle());
    m_Mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}

void Input::Update()
{
    m_KeyboardState = m_Keyboard->GetState();
    m_KeyboardTracker.Update(m_KeyboardState);

    m_MouseState = m_Mouse->GetState();
    m_MouseTracker.Update(m_MouseState);
}

bool Input::IsKeyDown(DirectX::Keyboard::Keys key) const
{
    return m_KeyboardState.IsKeyDown(key);
}

bool Input::IsKeyPressed(DirectX::Keyboard::Keys key) const
{
    return m_KeyboardTracker.IsKeyPressed(key);
}

bool Input::IsKeyReleased(DirectX::Keyboard::Keys key) const
{
    return m_KeyboardTracker.IsKeyReleased(key);
}

bool Input::IsLeftButtonDown() const
{
    return m_MouseState.leftButton;
}

bool Input::IsLeftButtonPressed() const
{
    return m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::IsRightButtonDown() const
{
    return m_MouseState.rightButton;
}

bool Input::IsRightButtonPressed() const
{
    return m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}