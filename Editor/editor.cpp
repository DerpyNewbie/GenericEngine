#include "pch.h"

#include "editor.h"
#include "editor_prefs.h"
#include "hierarchy.h"
#include "inspector.h"
#include "profiler.h"
#include "update_man_debugger.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "DxLib/dxlib_helper.h"
#include "update_manager.h"

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
void Editor::Init()
{
    {
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

        ImGui_ImplWin32_Init(GetMainWindowHandle());
        ImGui_ImplDX11_Init((ID3D11Device *)GetUseDirect3D11Device(),
                            (ID3D11DeviceContext *)GetUseDirect3D11DeviceContext());

        SetHookWinProc(WndProc);
    }

    {
        const auto hierarchy = std::make_shared<Hierarchy>();

        AddEditorWindow("Hierarchy", hierarchy);
        AddEditorWindow("Inspector", std::make_shared<Inspector>(&hierarchy->selected_game_object));
        AddEditorWindow("Editor Prefs", std::make_shared<EditorPrefs>());
        AddEditorWindow("Profiler", std::make_shared<Profiler>());
        AddEditorWindow("Update Manager Debugger", std::make_shared<UpdateManDebugger>());
    }
}
void Editor::Update()
{}

void Editor::Attach()
{
    engine::UpdateManager::SubscribeDrawCall(shared_from_base<Editor>());
}

void Editor::OnDraw()
{
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    if (EditorPrefs::theme != m_last_editor_style_)
        SetEditorStyle(EditorPrefs::theme);
    if (EditorPrefs::show_grid)
        DxLibHelper::DrawYPlaneGrid();

    for (const auto &window : m_editor_windows_ | std::views::values)
    {
        window->DrawGui();
    }

    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
    ImGui_ImplDX11_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}
void Editor::AddEditorWindow(const std::string &name, std::shared_ptr<EditorWindow> window)
{
    window->editor = this;
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
}