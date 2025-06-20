#include "application.h"
#include "pch.h"



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    g_app = new Application();
    g_app->StartApp();
}