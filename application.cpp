#include "pch.h"

#include "imgui_impl_win32.h"

#include "input.h"
#include "application.h"
#include <tchar.h>
#include "engine.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
int window_height = 1080;
int window_width = 1920;
HWND window_handle = nullptr;
std::string window_title = "GenericEngine";
bool play_mode = false;
}

namespace engine
{
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
        if (wparam == SIZE_MINIMIZED)
            break;

        window_width = LOWORD(lparam);
        window_height = HIWORD(lparam);

        on_window_resized.Invoke();
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

WPARAM Application::Run()
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

    Engine::Finalize();
    return msg.wParam;
}

int Application::WindowWidth()
{
    return window_width;
}

int Application::WindowHeight()
{
    return window_height;
}

HWND Application::WindowHandle()
{
    return window_handle;
}

std::string Application::WindowTitle()
{
    return window_title;
}

void Application::SetWindowTitle(const std::string &title)
{
    window_title = title;

    if (window_handle == nullptr)
        return;

    SetWindowText(window_handle, window_title.c_str());
}

bool Application::IsPlayMode()
{
    return play_mode;
}

void Application::InitWindow()
{
    // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEX w = {};

    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WindowProc);
    w.lpszClassName = _T("GenericEngine");
    w.hInstance = GetModuleHandle(nullptr);

    RegisterClassEx(&w);

    RECT wrc = {0, 0, static_cast<LONG>(window_width), static_cast<LONG>(window_height)};

    AdjustWindowRect(&wrc,WS_OVERLAPPEDWINDOW, false);

    window_handle = CreateWindow(w.lpszClassName,
                                 _T(window_title.c_str()),
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 wrc.right - wrc.left,
                                 wrc.bottom - wrc.top,
                                 nullptr,
                                 nullptr,
                                 w.hInstance,
                                 nullptr);

    ShowWindow(window_handle,SW_SHOW);
}

void Application::SetPlayMode(const bool play)
{
    play_mode = play;
}
}