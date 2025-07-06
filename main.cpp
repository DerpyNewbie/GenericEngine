#include "pch.h"
#include "application.h"

#include <d3d12sdklayers.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ID3D12Debug* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
    }
    g_app = new Application();
    g_app->StartApp();
}