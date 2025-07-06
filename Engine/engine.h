#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif

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