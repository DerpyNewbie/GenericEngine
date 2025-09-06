#include "pch.h"

#include "input.h"
#include "application.h"
#include <tchar.h>
#include "engine.h"
#include "game_object.h"
#include "Audio/audio_listener_component.h"

#include "Components/camera.h"
#include "Components/skinned_mesh_renderer.h"
#include "Editor/editor.h"
#include "Physics/plane_collider.h"

namespace engine
{
LRESULT Application::WndProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Input::ProcessMessage(msg, wparam, lparam);
    
    for (const auto callback : std::views::values(Instance()->m_callbacks_))
    {
        return callback(hwnd, msg, wparam, lparam);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

std::shared_ptr<Application> Application::Instance()
{
    static auto instance = std::make_shared<Application>();
    return instance;
}

void Application::StartApp()
{
    const auto engine = std::make_shared<Engine>();
    const auto editor = std::make_shared<editor::Editor>();
    InitWindow();
    engine->Init();
    editor->Init();
    editor->Attach();

    {
        // Sample scene creation
        const auto camera = Object::Instantiate<GameObject>("Camera");
        camera->AddComponent<Camera>();
        camera->AddComponent<AudioListenerComponent>();

        Object::Instantiate<GameObject>("Floor")->AddComponent<PlaneCollider>();

        for (auto &func : m_initial_scene_creation_)
        {
            func();
        }
    }

    engine->MainLoop();

    editor->Finalize();
}

int Application::WindowWidth() const
{
    return m_window_width_;
}

int Application::WindowHeight() const
{
    return m_window_height_;
}

HWND Application::GetWindowHandle() const
{
    return m_h_wnd_;
}

int Application::AddWindowCallback(std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)> callback)
{
    int handle = static_cast<int>(reinterpret_cast<intptr_t>(&callback));
    m_callbacks_.emplace(handle, callback);
    return handle;
}

void Application::RemoveWindowCallback(const int window_callback_handle)
{
    m_callbacks_.erase(window_callback_handle);
}

void Application::AddInitialSceneCreationCallback(std::function<void()> func)
{
    m_initial_scene_creation_.emplace_back(func);
}

void Application::InitWindow()
{
    WNDCLASSEX w = {};

    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WndProcedure);
    w.lpszClassName = _T("GenericEngine");
    w.hInstance = GetModuleHandle(nullptr);

    RegisterClassEx(&w);

    RECT wrc = {0, 0, static_cast<LONG>(m_window_width_), static_cast<LONG>(m_window_height_)};

    AdjustWindowRect(&wrc,WS_OVERLAPPEDWINDOW, false);

    m_h_wnd_ = CreateWindow(w.lpszClassName,
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

    ShowWindow(m_h_wnd_,SW_SHOW);
}
}