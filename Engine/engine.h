#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif

#include "engine_profiler.h"
#include "scene_manager.h"
#include "engine_time.h"
#include "update_manager.h"
#include <DxLib.h>

namespace engine
{
class Engine
{
public:
    bool should_exit = false;

    bool Init()
    {
#ifdef _DEBUG
        LoadLibraryExA("assimp-vc143-mtd.dll", nullptr, NULL);
#else
        LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif

        SetGraphMode(1280, 720, 32);
        ChangeWindowMode(TRUE);
        if (DxLib_Init() == -1)
        {
            should_exit = true;
            return false;
        }

        SetUseZBuffer3D(true);

        Time::Get()->Init();
        SceneManager::CreateScene("Default Scene");

        return true;
    }

    void MainLoop() const
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
            ClearDrawScreen();
            UpdateManager::InvokeDrawCall();

            {
                // temporary engine info drawer
                const auto time = Time::Get();
                DrawFormatString(20, 20, 0xffffffff, "fps: %3d, f: %03d, dt: %f",
                                 time->Fps(), time->Frames() % 1000, time->DeltaTime());
                DrawFormatString(20, 60, 0xffffffff, "update: %d, fixed_update: %d, draw_call: %d",
                                 UpdateManager::UpdateCount(), UpdateManager::FixedUpdateCount(),
                                 UpdateManager::DrawCallCount());
            }
            ScreenFlip();
            Profiler::End("Draw Call");
        }
    }
};
}