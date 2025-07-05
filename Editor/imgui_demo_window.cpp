#include "pch.h"
#include "imgui_demo_window.h"

namespace editor
{
std::string ImGuiDemoWindow::Name()
{
    return "ImGui Demo Window";
}
void ImGuiDemoWindow::OnEditorGui()
{
    ImGui::ShowDemoWindow();
}
}