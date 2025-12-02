#include "pch.h"
#include "imgui_demo_window.h"

namespace editor
{
bool ImGuiDemoWindow::ShouldHandleWindowAutomatically() const
{
    return false;
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