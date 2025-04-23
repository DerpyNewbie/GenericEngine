#pragma once
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
        SetGraphMode(800, 400, 32);
        ChangeWindowMode(TRUE);
        if (DxLib_Init() == -1)
        {
            should_exit = true;
            return false;
        }

        Time::Get()->Init();

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