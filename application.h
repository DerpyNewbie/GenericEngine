#pragma once
#include "event.h"

#include <Windows.h>
#include <functional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DescriptorHandle;

namespace engine
{
class Application
{
    /// @brief Window initialization
    static void InitWindow();

public:
    static Event<> on_window_resized;

    static std::shared_ptr<Application> Instance();

    /// @brief Start application
    static WPARAM Run();

    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static int WindowWidth();
    static int WindowHeight();
    static HWND WindowHandle();
    static std::string WindowTitle();
    static void SetWindowTitle(const std::string &title);
};
}