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
    static int m_window_height_;
    static int m_window_width_;
    static HWND m_window_handle_;

    /// @brief Window initialization
    static void InitWindow();

public:
    static Event<> on_window_resized;

    static std::shared_ptr<Application> Instance();

    /// @brief Start application
    static WPARAM StartApp();

    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static int WindowWidth();
    static int WindowHeight();
    static HWND WindowHandle();
};
}