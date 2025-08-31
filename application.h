#pragma once
#include <Windows.h>
#include <functional>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DescriptorHandle;

class Application
{
    typedef std::function<LRESULT (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)> WindowCallback;

    static std::unordered_map<int, WindowCallback> m_callbacks_;
    static int m_window_height_;
    static int m_window_width_;
    static HWND m_h_wnd_;

    /// @brief Window initialization
    static void InitWindow();

    static std::shared_ptr<Application> m_instance_;

public:
    static std::shared_ptr<Application> Instance();
    /// @brief Start application
    void StartApp();

    static LRESULT WndProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static int WindowWidth();
    static int WindowHeight();
    static HWND GetWindowHandle();

    /// @returns WindowCallback Handle
    static int AddWindowCallback(WindowCallback callback);
    static void RemoveWindowCallback(int window_callback_handle);
};