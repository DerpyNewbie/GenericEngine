#include "App.h"

#include "Engine.h"
#include "Scene.h"
#include "Components/camera.h"
#include "Components/controller.h"
#include "Editor/editor.h"
#include "Engine/Rendering/CabotEngine/Core/Input.h"
#include "Rendering/model_importer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"


Application* g_App;

void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
    va_list valist;
    va_start(valist, format);
    printf(format, valist);
    va_end(valist);
#endif
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    for (auto callback : Application::callbacks)
    {
        callback(hwnd, msg, wparam, lparam);
    }
    if (g_pInput && g_pInput->m_Keyboard)
    {
        g_pInput->m_Keyboard->ProcessMessage(msg, wparam, lparam);
    }

    if (msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam); //規定の処理を行う
}

void Application::InitWindow()
{
    WNDCLASSEX w = {};

    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = static_cast<WNDPROC>(WindowProcedure); //コールバック関数の指定
    w.lpszClassName = _T("DX12Sample"); //アプリケーションクラス名
    w.hInstance = GetModuleHandle(nullptr); //ハンドルの取得

    RegisterClassEx(&w); //アプリケーションクラス(ウィンドウクラスの指定をOSに伝える)

    RECT wrc = {0, 0, kWindowWidth, kWindowHeight};

    //関数を使ってウィンドウのサイズを補正する
    AdjustWindowRect(&wrc,WS_OVERLAPPEDWINDOW, false);

    //ウィンドウオブジェクトの生成
    hWnd = CreateWindow(w.lpszClassName,
                        _T("DX12テスト"),
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
    ShowWindow(hWnd,SW_SHOW);
}

void Application::StartApp()
{
    const auto engine = std::make_shared<engine::Engine>();
    const auto editor = std::make_shared<editor::Editor>();

    editor->Init();
    editor->Attach();
    InitWindow();
    engine->Init();
    
    {
        // Sample scene creation
        const auto camera = engine::Object::Instantiate<engine::GameObject>("Camera");
        camera->AddComponent<engine::Camera>();
        camera->AddComponent<engine::Controller>();

        const auto parent_obj = engine::Object::Instantiate<engine::GameObject>("Parent");
        const auto child_obj = engine::Object::Instantiate<engine::GameObject>("Child");
        child_obj->Transform()->SetParent(parent_obj->Transform());

        const auto parent_obj2 = engine::Object::Instantiate<engine::GameObject>("Parent2");
        const auto child_obj2 = engine::Object::Instantiate<engine::GameObject>("Child2");
        child_obj2->Transform()->SetParent(parent_obj2->Transform());

        engine::ModelImporter::LoadModelFromFBX("Resources/primitives/cube.fbx");
        engine::ModelImporter::LoadModelFromFBX("Resources/primitives/submesh_cube.fbx");
        engine::ModelImporter::LoadModelFromFBX("Resources/hackadoll/hackadoll.fbx");
    }

    // メイン処理ループ
    engine->MainLoop();

    editor->Finalize();
}
