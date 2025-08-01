#include "pch.h"
#include "application.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Application::Instance()->StartApp();
}