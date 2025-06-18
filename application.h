#pragma once
#include <Windows.h>
#include <functional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class Application
{
    typedef std::function<LRESULT (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)> WindowCallback;

public:
    /// @brief アプリケーションのスタート
    void StartApp();

    static int WindowWidth();
    static int WindowHeight();
    static HWND GetWindowHandle();

    /// @returns WindowCallback Handle
    static int AddWindowCallback(WindowCallback callback);
    static void RemoveWindowCallback(int window_callback_handle);

private:
    static std::unordered_map<int, WindowCallback> m_callbacks_;
    static float m_window_height_;
    static float m_window_width_;
    static HWND m_h_wnd_;

    /// @brief ウィンドウの初期化
    void InitWindow();

};

extern Application *g_app;