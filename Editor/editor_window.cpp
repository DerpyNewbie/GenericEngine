#include "editor_window.h"

#include "imgui.h"

void editor::IEditorWindow::DrawGui()
{
    if (ImGui::Begin(Name().c_str(), nullptr, window_flags))
    {
        OnEditorGui();
    }
    ImGui::End();
}