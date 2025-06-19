#include "pch.h"

#include "editor_window.h"

void editor::IEditorWindow::DrawGui()
{
    if (!is_open)
        return;

    if (ImGui::Begin(Name().c_str(), nullptr, window_flags))
    {
        OnEditorGui();
    }
    ImGui::End();
}