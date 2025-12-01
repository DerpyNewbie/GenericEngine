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

    if (m_handle_window_manually_)
    {
        OnEditorGui();
    }
    else
    {
        if (ImGui::Begin(Name().c_str(), nullptr, window_flags))
        {
            OnEditorGui();
        }
        ImGui::End();
    }
}