#pragma once
#include <DxLib.h>
#include <string>

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

    void MainLoop()
    {
        while (ProcessMessage() == 0 && !should_exit)
        {
            // TODO: implement engine loop
        }
    }
};
}