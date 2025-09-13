#include "pch.h"

#include "imgui_impl_win32.h"

#include "input.h"
#include "application.h"
#include <tchar.h>
#include "engine.h"
#include "Physics/plane_collider.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine
{
int Application::m_window_height_ = 1080;
int Application::m_window_width_ = 1920;
HWND Application::m_window_handle_ = nullptr;
Event<> Application::on_window_resized;

LRESULT Application::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Logger::Log<Application>("Window destroyed");
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        Logger::Log<Application>("Window resized");
        if (wparam == SIZE_MINIMIZED)
            break;
        Instance()->m_window_width_ = LOWORD(lparam);
        Instance()->m_window_height_ = HIWORD(lparam);
        Instance()->on_window_resized.Invoke();
        break;
    case WM_MOUSEACTIVATE:
        Logger::Log<Application>("Mouse activated");
        return MA_ACTIVATEANDEAT;
    default:
        break;
    }

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    Input::ProcessMessage(msg, wparam, lparam);

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

std::shared_ptr<Application> Application::Instance()
{
    static auto instance = std::make_shared<Application>();
    return instance;
}

WPARAM Application::StartApp()
{
    InitWindow();
    Engine::Init();
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        Engine::Tick();
    }

    return msg.wParam;
}

int Application::WindowWidth()
{
    return m_window_width_;
}

int Application::WindowHeight()
{
    return m_window_height_;
}

HWND Application::WindowHandle()
{
    return m_window_handle_;
}

void Application::InitWindow()
{
    WNDCLASSEX w = {};

    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WindowProc);
    w.lpszClassName = _T("GenericEngine");
    w.hInstance = GetModuleHandle(nullptr);

    RegisterClassEx(&w);

    RECT wrc = {0, 0, static_cast<LONG>(m_window_width_), static_cast<LONG>(m_window_height_)};

    AdjustWindowRect(&wrc,WS_OVERLAPPEDWINDOW, false);

    m_window_handle_ = CreateWindow(w.lpszClassName,
                                    _T("GenericEngine"),
                                    WS_OVERLAPPEDWINDOW,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    wrc.right - wrc.left,
                                    wrc.bottom - wrc.top,
                                    nullptr,
                                    nullptr,
                                    w.hInstance,
                                    nullptr);

    ShowWindow(m_window_handle_,SW_SHOW);
}
}