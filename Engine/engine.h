#pragma once
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

        return true;
    }

    void MainLoop() const
    {
        while (ProcessMessage() == 0 && !should_exit)
        {
            UpdateManager::InvokeUpdate();
            // TODO: make fixed update actually fixed
            UpdateManager::InvokeFixedUpdate();
            UpdateManager::InvokeDrawCall();
        }
    }
};
}