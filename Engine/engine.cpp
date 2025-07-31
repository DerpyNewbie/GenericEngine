#include "pch.h"

#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif

#include "engine.h"
#include "component_factory.h"
#include "Asset/asset_database.h"
#include "engine_profiler.h"
#include "engine_time.h"
#include "update_manager.h"
#include "scene_manager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "application.h"
#include "Rendering/gizmos.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "input.h"

namespace engine
{
bool Engine::Init()
{
#ifdef _DEBUG
    LoadLibraryExA("assimp-vc143-mtd.dll", nullptr, NULL);
#else
    LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif
    g_RenderEngine = new RenderEngine();
    if (!g_RenderEngine->Init(Application::GetWindowHandle(), Application::WindowWidth(), Application::WindowHeight()))
    {
        Logger::Log<Engine>("Failed to initialize render engine");
    }

    Gizmos::Init();
    Time::Get()->Init();
    Input::Instance()->Init();
    AssetDatabase::Init();
    IComponentFactory::Init();
    SceneManager::CreateScene("Default Scene");

    return true;
}

void Engine::MainLoop() const
{
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE == TRUE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        Profiler::NewFrame();
        Time::Get()->IncrementFrame();

        Profiler::Begin("Fixed Update");
        const auto fixed_update_count = Time::Get()->UpdateFixedFrameCount();
        for (int i = 0; i < fixed_update_count; i++)
        {
            UpdateManager::InvokeFixedUpdate();
        }
        Profiler::End("Fixed Update");

        Profiler::Begin("Update");
        Input::Instance()->Update();
        UpdateManager::InvokeUpdate();
        Profiler::End("Update");

        Profiler::Begin("Draw Call");
        g_RenderEngine->BeginRender();
        UpdateManager::InvokeDrawCall();
        g_RenderEngine->EndRender();
        Profiler::End("Draw Call");

        Profiler::Begin("Cleanup Objects");
        Object::GarbageCollect();
        Profiler::End("Cleanup Objects");

    }
}
}