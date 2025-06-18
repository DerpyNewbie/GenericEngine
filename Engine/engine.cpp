#include "pch.h"

#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif

#include "engine.h"

#include "engine_profiler.h"
#include "engine_time.h"
#include "update_manager.h"
#include "scene_manager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "application.h"

#include <DxLib.h>

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

    if (DxLib_Init() == -1)
    {
        should_exit = true;
        return false;
    }

    Time::Get()->Init();
    SceneManager::CreateScene("Default Scene");

    return true;
}
void Engine::MainLoop() const
{
    while (ProcessMessage() == 0 && !should_exit)
    {
        Profiler::NewFrame();
        Time::Get()->IncrementFrame();

        Profiler::Begin("Update");
        UpdateManager::InvokeUpdate();
        Profiler::End("Update");

        Profiler::Begin("Fixed Update");
        const auto fixed_update_count = Time::Get()->UpdateFixedFrameCount();
        for (int i = 0; i < fixed_update_count; i++)
        {
            UpdateManager::InvokeFixedUpdate();
        }
        Profiler::End("Fixed Update");

        Profiler::Begin("Draw Call");
        g_RenderEngine->BeginRender();
        UpdateManager::InvokeDrawCall();
        g_RenderEngine->EndRender();
        Profiler::End("Draw Call");
    }
}
}