#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif
#include "event.h"

namespace engine
{
class Engine
{
public:
    static Event<> on_init;
    static Event<> on_default_scene_creation;
    static Event<> on_tick;
    static Event<> on_finalize;

    static bool Init();
    static void Tick();
    static void Finalize();
};
}