#include "InputManager.h"
#include "application.h"
InputManager g_Input;

InputManager::InputManager()
{
    m_Keyboard = std::make_unique<DirectX::Keyboard>();
    m_Mouse = std::make_unique<DirectX::Mouse>();
}

void InputManager::Initialize()
{
    m_Mouse->SetWindow(Application::GetWindowHandle());
    m_Mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}

void InputManager::Update()
{
    m_KeyboardState = m_Keyboard->GetState();
    m_KeyboardTracker.Update(m_KeyboardState);

    m_MouseState = m_Mouse->GetState();
    m_MouseTracker.Update(m_MouseState);
}

bool InputManager::IsKeyDown(DirectX::Keyboard::Keys key) const
{
    return m_KeyboardState.IsKeyDown(key);
}

bool InputManager::IsKeyPressed(DirectX::Keyboard::Keys key) const
{
    return m_KeyboardTracker.IsKeyPressed(key);
}

bool InputManager::IsKeyReleased(DirectX::Keyboard::Keys key) const
{
    return m_KeyboardTracker.IsKeyReleased(key);
}

bool InputManager::IsLeftButtonDown() const
{
    return m_MouseState.leftButton;
}

bool InputManager::IsLeftButtonPressed() const
{
    return m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool InputManager::IsRightButtonDown() const
{
    return m_MouseState.rightButton;
}

bool InputManager::IsRightButtonPressed() const
{
    return m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

Vector2 InputManager::MousePos()
{
    return {static_cast<float>(m_MouseState.x), static_cast<float>(m_MouseState.y)};
}