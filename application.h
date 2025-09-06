#pragma once
#include <Windows.h>
#include <functional>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DescriptorHandle;
namespace engine
{
class Application
{
    typedef std::function<LRESULT (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)> WindowCallback;

    std::unordered_map<int, WindowCallback> m_callbacks_;
    std::list<std::function<void()>> m_initial_scene_creation_;
    int m_window_height_ = 1080;
    int m_window_width_ = 1920;
    HWND m_h_wnd_ = nullptr;

    /// @brief Window initialization
    void InitWindow();

public:
    static std::shared_ptr<Application> Instance();
    /// @brief Start application
    void StartApp();

    static LRESULT WndProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    int WindowWidth() const;
    int WindowHeight() const;
    HWND GetWindowHandle() const;

    /// @returns WindowCallback Handle
    int AddWindowCallback(WindowCallback callback);
    void RemoveWindowCallback(int window_callback_handle);

    void AddInitialSceneCreationCallback(std::function<void()> func);
};
}