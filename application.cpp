#include "pch.h"

#include "application.h"

std::unordered_map<int, Application::WindowCallback> Application::m_callbacks_;
float Application::m_window_height_ = 1080;
float Application::m_window_width_ = 1920;
HWND Application::m_h_wnd_ = nullptr;

void Application::StartApp()
{
    // TODO: implement
    assert(false && "not implemented");
}
int Application::WindowWidth()
{
    return m_window_width_;
}
int Application::WindowHeight()
{
    return m_window_height_;
}
HWND Application::GetWindowHandle()
{
    return m_h_wnd_;
}
int Application::AddWindowCallback(std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)> callback)
{
    int handle = reinterpret_cast<int>(&callback);
    m_callbacks_.emplace(handle, callback);
    return handle;
}
void Application::RemoveWindowCallback(int window_callback_handle)
{
    m_callbacks_.erase(window_callback_handle);
}
void Application::InitWindow()
{
    // TODO: implement
    assert(false && "not implemented");
}