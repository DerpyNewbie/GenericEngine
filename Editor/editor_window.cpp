#include "editor_window.h"

#include "imgui.h"

void editor::IEditorWindow::DrawGui()
{
    if (ImGui::Begin(Name().c_str()))
    {
        OnEditorGui();
    }
    ImGui::End();
}