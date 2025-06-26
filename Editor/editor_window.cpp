#include "pch.h"

#include "editor_window.h"

std::string editor::EditorWindow::Name()
{
    return typeid(*this).name();
}
void editor::EditorWindow::DrawGui()
{
    if (!is_open)
        return;

    if (ImGui::Begin(Name().c_str(), nullptr, window_flags))
    {
        OnEditorGui();
    }
    ImGui::End();
}