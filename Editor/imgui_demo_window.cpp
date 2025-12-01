#include "pch.h"
#include "imgui_demo_window.h"

namespace editor
{
ImGuiDemoWindow::ImGuiDemoWindow()
{
    m_handle_window_manually_ = true;
}

std::string ImGuiDemoWindow::Name()
{
    return "ImGui Demo Window";
}

void ImGuiDemoWindow::OnEditorGui()
{
    ImGui::ShowDemoWindow();
}
}