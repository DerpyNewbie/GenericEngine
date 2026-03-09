#include "pch.h"
#include "bootstrap.h"

using namespace engine;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
    return static_cast<int>(bootstrap::Launch(bootstrap::LaunchOptions::Parse(lpCmdLine)));
}