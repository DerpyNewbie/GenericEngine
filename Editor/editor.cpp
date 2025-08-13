#include "pch.h"

#include "editor.h"

#include "asset_browser.h"
#include "default_editor_menus.h"
#include "editor_prefs.h"
#include "hierarchy.h"
#include "imgui_demo_window.h"
#include "inspector.h"
#include "profiler.h"
#include "update_man_debugger.h"

#include "application.h"
#include "audio_window.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "DxLib/dxlib_helper.h"
#include "update_manager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Asset/text_asset.h"
#include "Rendering/material.h"
#include "Rendering/render_texture.h"
#include "Physics/physics.h"

#include <ranges>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SetUseHookWinProcReturnValue(true);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace editor
{
using namespace engine;

Editor *Editor::m_instance_ = nullptr;

void Editor::SetEditorStyle(const int i)
{
    if (m_last_editor_style_ == i)
        return;
    switch (i)
    {
    default:
    case 0:
        ImGui::StyleColorsDark();
        break;
    case 1:
        ImGui::StyleColorsLight();
        break;
    case 2:
        ImGui::StyleColorsClassic();
        break;
    }

    m_last_editor_style_ = i;
}

Editor *Editor::Instance()
{
    return m_instance_;
}

void Editor::Init()
{
    m_instance_ = this;

    {
        Application::AddWindowCallback(WndProc);
        // imgui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        SetEditorStyle(0);

        ImFontConfig font_config;
        font_config.MergeMode = true;
        font_config.GlyphRanges = io.Fonts->GetGlyphRangesJapanese();
        io.Fonts->AddFontDefault();
        ImFont *font = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\meiryo.ttc)", 18.0f, &font_config,
                                                    io.Fonts->GetGlyphRangesJapanese());
        IM_ASSERT(font != nullptr);

        auto descriptor_handle = DescriptorHeap::Allocate();
        // フォントディスクリプタ取得
        D3D12_CPU_DESCRIPTOR_HANDLE font_cpu_desc_handle = descriptor_handle->HandleCPU;
        D3D12_GPU_DESCRIPTOR_HANDLE font_gpu_desc_handle = descriptor_handle->HandleGPU;

        ImGui_ImplWin32_Init(Application::GetWindowHandle());
        ImGui_ImplDX12_Init(
            g_RenderEngine->Device(),
            RenderEngine::FRAME_BUFFER_COUNT,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DescriptorHeap::GetHeap(),
            font_cpu_desc_handle,
            font_gpu_desc_handle
            );

        SetHookWinProc(WndProc);
    }

    {
        AddEditorWindow("Hierarchy", std::make_shared<Hierarchy>());
        AddEditorWindow("Inspector", std::make_shared<Inspector>());
        AddEditorWindow("Editor Prefs", std::make_shared<EditorPrefs>());
        AddEditorWindow("Profiler", std::make_shared<Profiler>());
        AddEditorWindow("Update Manager Debugger", std::make_shared<UpdateManDebugger>());
        AddEditorWindow("Asset Browser", std::make_shared<AssetBrowser>());
        AddEditorWindow("ImGui Demo Window", std::make_shared<ImGuiDemoWindow>());
        AddEditorWindow("Audio", std::make_shared<AudioWindow>());
    }

    {
        const auto default_menu = std::make_shared<DefaultEditorMenu>();
        AddEditorMenu("File", default_menu, -1000);
        AddEditorMenu("Edit", default_menu, -990);
        AddEditorMenu("Asset", default_menu, -980);
        AddEditorMenu("GameObject", default_menu, -970);
        AddEditorMenu("Component", default_menu, -960);
        AddEditorMenu("Window", default_menu, -950);
    }

    {
        AddCreateMenu("Text Asset", ".txt", [] {
            return Object::Instantiate<TextAsset>("New Text Asset");
        });

        AddCreateMenu("Material", ".material", [] {
            return Object::Instantiate<Material>("New Material");
        });
        AddCreateMenu("Render Texture", ".rendertexture", [] {
            return Object::Instantiate<RenderTexture>("New RenderTexture");
        });
    }
}

void Editor::Attach()
{
    UpdateManager::SubscribeDrawCall(shared_from_base<Editor>());
}

void Editor::OnDraw()
{
    {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
    }

    if (ImGui::BeginMainMenuBar())
    {
        DrawEditorMenuBar();
        ImGui::EndMainMenuBar();
    }

    if (EditorPrefs::theme != m_last_editor_style_)
        SetEditorStyle(EditorPrefs::theme);
    if (EditorPrefs::show_grid)
        DxLibHelper::DrawYPlaneGrid();
    if (EditorPrefs::show_physics_debug)
        Physics::DebugDraw();

    for (const auto &window : m_editor_windows_ | std::views::values)
    {
        window->DrawGui();
    }

    {
        ImGui::Render();
        auto draw_data = ImGui::GetDrawData();
        auto cmd_list = g_RenderEngine->CommandList();
        ImGui_ImplDX12_RenderDrawData(draw_data, cmd_list);
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    RefreshDxLibDirect3DSetting();
}

void Editor::Finalize()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX12_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void Editor::SetSelectedObject(const std::shared_ptr<Object> &object)
{
    m_selected_object_ = object;
}

std::shared_ptr<Object> Editor::SelectedObject() const
{
    return m_selected_object_.lock();
}

void Editor::SetSelectedDirectory(const std::filesystem::path &path)
{
    m_selected_directory_ = path;
}

std::filesystem::path Editor::SelectedDirectory() const
{
    return m_selected_directory_;
}

void Editor::AddEditorWindow(const std::string &name, std::shared_ptr<EditorWindow> window)
{
    m_editor_windows_.emplace(name, window);
}

std::vector<std::string> Editor::GetEditorWindowNames()
{
    auto keys = std::views::keys(m_editor_windows_);
    return {keys.begin(), keys.end()};
}

std::shared_ptr<EditorWindow> Editor::GetEditorWindow(const std::string &name)
{
    return m_editor_windows_.at(name);
}

void Editor::RemoveEditorWindow(const std::string &name)
{
    m_editor_windows_.erase(name);
}

void Editor::AddEditorMenu(const std::string &name, const std::shared_ptr<EditorMenu> &menu, const int priority)
{
    menu->editor = this;
    m_editor_menus_.emplace_back(name, menu, priority);
    std::ranges::sort(m_editor_menus_, std::ranges::less(), &PrioritizedEditorMenu::priority);
}

std::vector<Editor::PrioritizedEditorMenu> Editor::GetEditorMenus()
{
    return m_editor_menus_;
}

std::shared_ptr<EditorMenu> Editor::GetEditorMenu(const std::string &name)
{
    const auto item = std::ranges::find(m_editor_menus_, name, &PrioritizedEditorMenu::name);
    return item->menu;
}

void Editor::RemoveEditorMenu(const std::string &name)
{
    m_editor_menus_.erase(std::ranges::find_if(m_editor_menus_, [&](const auto &m) {
        return m.name == name;
    }));
}

void Editor::AddCreateMenu(const std::string &name, const std::string &extension,
                           std::function<std::shared_ptr<Object>()> factory, int priority)
{
    m_create_menus_.emplace_back(name, extension, factory, priority);
    std::ranges::sort(m_create_menus_, std::ranges::less(), &PrioritizedCreateMenu::priority);
}

std::vector<Editor::PrioritizedCreateMenu> Editor::GetCreateMenus()
{
    return m_create_menus_;
}

void Editor::RemoveCreateMenu(const std::string &name)
{
    m_create_menus_.erase(std::ranges::find_if(m_create_menus_, [&](const auto &m) {
        return m.name == name;
    }));
}

void Editor::DrawEditorMenuBar() const
{
    for (const auto &menu : m_editor_menus_)
    {
        if (ImGui::BeginMenu(menu.name.c_str()))
        {
            menu.menu->OnEditorMenuGui(menu.name);
            ImGui::EndMenu();
        }
    }
}

void Editor::DrawEditorMenu(const std::string &name)
{
    GetEditorMenu(name)->OnEditorMenuGui(name);
}
}