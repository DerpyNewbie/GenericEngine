#include "pch.h"
#include "application.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    engine::Application::Instance()->StartApp();
}