#pragma once
#include <Windows.h>
#include <functional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class Application
{
public:
    static std::vector<std::function<LRESULT (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)>> callbacks;
    /// @brief アプリケーションのスタート
    void StartApp();

    static int WindowWidth() { return kWindowWidth; }
    static int WindowHeight() { return kWindowHeight; }
    static HWND GetHWnd() { return hWnd; }

private:
    constexpr static int kWindowWidth = 1920;
    constexpr static int kWindowHeight = 1080;

    static HWND hWnd;

private:
    /// @brief ウィンドウの初期化
    void InitWindow();

};

extern Application* g_App;
