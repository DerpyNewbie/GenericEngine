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
    
    /// @brief Window initialization
    static void InitWindow();
    static void SetPlayMode(bool play);

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
    static bool IsPlayMode();
};
}