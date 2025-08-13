#include "pch.h"

#include "application.h"
#include <tchar.h>
#include "engine.h"
#include "game_object.h"
#include "input.h"
#include "Audio/audio_listener_component.h"

#include "Components/camera.h"
#include "Components/controller.h"
#include "Components/skinned_mesh_renderer.h"
#include "Rendering/model_importer.h"
#include "Editor/editor.h"

std::shared_ptr<Application> Application::m_instance_;
std::unordered_map<int, Application::WindowCallback> Application::m_callbacks_;
float Application::m_window_height_ = 1080;
float Application::m_window_width_ = 1920;
HWND Application::m_h_wnd_ = nullptr;

LRESULT Application::WndProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (engine::Input::Instance()->Keyboard())
    {
        engine::Input::Instance()->Keyboard()->ProcessMessage(msg, wparam, lparam);
    }
    for (auto callback : std::views::values(m_callbacks_))
    {
        return callback(hwnd, msg, wparam, lparam);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

std::shared_ptr<Application> Application::Instance()
{
    return m_instance_;
}

void Application::StartApp()
{
    const auto engine = std::make_shared<engine::Engine>();
    const auto editor = std::make_shared<editor::Editor>();
    InitWindow();
    engine->Init();
    editor->Init();
    editor->Attach();

    {
        // Sample scene creation
        const auto camera = engine::Object::Instantiate<engine::GameObject>("Camera");
        camera->AddComponent<engine::Camera>();
        camera->AddComponent<engine::Controller>();
        camera->AddComponent<engine::AudioListenerComponent>();

        const auto parent_obj = engine::Object::Instantiate<engine::GameObject>("Parent");
        const auto child_obj = engine::Object::Instantiate<engine::GameObject>("Child");
        child_obj->Transform()->SetParent(parent_obj->Transform());

        const auto parent_obj2 = engine::Object::Instantiate<engine::GameObject>("Parent2");
        const auto child_obj2 = engine::Object::Instantiate<engine::GameObject>("Child2");
        child_obj2->Transform()->SetParent(parent_obj2->Transform());

        //engine::ModelImporter::LoadModelFromFBX("Resources/primitives/cube.fbx");
        //engine::ModelImporter::LoadModelFromFBX("Resources/primitives/submesh_cube.fbx");
        engine::ModelImporter::LoadModelFromFBX("Resources/hackadoll/hackadoll.fbx");
    }

    engine->MainLoop();

    editor->Finalize();
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
    WNDCLASSEX w = {};

    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WndProcedure); //コールバック関数の指定
    w.lpszClassName = _T("GenericEngine"); //アプリケーションクラス名
    w.hInstance = GetModuleHandle(nullptr); //ハンドルの取得

    RegisterClassEx(&w); //アプリケーションクラス(ウィンドウクラスの指定をOSに伝える)

    RECT wrc = {0, 0, static_cast<LONG>(m_window_width_), static_cast<LONG>(m_window_height_)};

    //関数を使ってウィンドウのサイズを補正する
    AdjustWindowRect(&wrc,WS_OVERLAPPEDWINDOW, false);

    //ウィンドウオブジェクトの生成
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

    //ウィンドウ表示
    ShowWindow(m_h_wnd_,SW_SHOW);
}