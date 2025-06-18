#include "pch.h"

#include "editor.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "DxLib/dxlib_helper.h"
#include "update_manager.h"

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

        ImGui::StyleColorsDark();

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
        m_hierarchy_ = engine::Object::Instantiate<Hierarchy>();

        m_inspector_ = engine::Object::Instantiate<Inspector>();
        m_inspector_->selected_game_object_ptr = &m_hierarchy_->selected_game_object;

        m_profiler_ = engine::Object::Instantiate<Profiler>();
        m_update_man_debugger_ = engine::Object::Instantiate<UpdateManDebugger>();
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

    DxLibHelper::DrawYPlaneGrid();

    m_hierarchy_->DrawGui();
    m_inspector_->DrawGui();
    m_profiler_->DrawGui();
    m_update_man_debugger_->DrawGui();

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
}