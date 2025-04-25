#pragma once
#include "scene_manager.h"
#include "time.h"
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
        SetGraphMode(1280, 720, 32);
        ChangeWindowMode(TRUE);
        if (DxLib_Init() == -1)
        {
            should_exit = true;
            return false;
        }

        Time::Get()->Init();
        SceneManager::CreateScene("Default Scene");

        return true;
    }

    void MainLoop() const
    {
        while (ProcessMessage() == 0 && !should_exit)
        {
            Time::Get()->IncrementFrame();
            UpdateManager::InvokeUpdate();

            const auto fixed_update_count = Time::Get()->UpdateFixedFrameCount();
            for (int i = 0; i < fixed_update_count; i++)
            {
                UpdateManager::InvokeFixedUpdate();
            }

            UpdateManager::InvokeDrawCall();
        }
    }
};
}