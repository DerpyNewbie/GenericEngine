#pragma once
#include "event.h"

#include <Windows.h>
#include <functional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace editor
{
class Editor;
}

class DescriptorHandle;

namespace engine
{
class Application
{
    friend class editor::Editor;

    static int m_window_height_;
    static int m_window_width_;
    static HWND m_window_handle_;
    static bool m_play_mode_;

    /// @brief Window initialization
    static void InitWindow();
    static void SetPlayMode(bool play);

public:
    static Event<> on_window_resized;

    static std::shared_ptr<Application> Instance();

    /// @brief Start application
    static WPARAM StartApp();

    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static int WindowWidth();
    static int WindowHeight();
    static HWND WindowHandle();
    static bool IsPlayMode();
};
}