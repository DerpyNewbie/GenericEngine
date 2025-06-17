#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif

#include "engine_time.h"

namespace engine
{
class Engine
{
public:
    bool should_exit = false;

    bool Init();

    void MainLoop() const;
};
}