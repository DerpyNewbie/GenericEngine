#include "pch.h"
#include "application.h"

#include <d3d12sdklayers.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Application::Instance()->StartApp();
}