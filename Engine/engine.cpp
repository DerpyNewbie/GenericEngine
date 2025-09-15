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
#include "input.h"
#include "Physics/physics.h"
#include "Audio/audio.h"
#include "Rendering/render_pipeline.h"

namespace engine
{
Event<> Engine::on_init;
Event<> Engine::on_default_scene_creation;
Event<> Engine::on_tick;
Event<> Engine::on_finalize;

bool Engine::Init()
{
#ifdef _DEBUG
    LoadLibraryExA("assimp-vc143-mtd.dll", nullptr, NULL);
#else
    LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif
    if (!RenderEngine::Instance()->Init(Application::Instance()->WindowHandle(),
                                        Application::Instance()->WindowWidth(),
                                        Application::Instance()->WindowHeight()))
    {
        Logger::Log<Engine>("Failed to initialize render engine");
    }

    Audio::Init();
    Gizmos::Init();
    Physics::Init();
    Time::Get()->Init();
    Input::Instance()->Init();
    AssetDatabase::Init();
    IComponentFactory::Init();
    on_init.Invoke();

    SceneManager::CreateScene("Default Scene");
    on_default_scene_creation.Invoke();
    return true;
}

void Engine::Tick()
{
    Profiler::NewFrame();
    Time::Get()->IncrementFrame();

    on_tick.Invoke();

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
    RenderEngine::Instance()->BeginRender();
    RenderPipeline::Instance()->InvokeDrawCall();
    RenderEngine::Instance()->EndRender();
    Profiler::End("Draw Call");

    Profiler::Begin("Cleanup Objects");
    Object::GarbageCollect();
    Profiler::End("Cleanup Objects");
}

void Engine::Finalize()
{
    on_finalize.Invoke();
}
}