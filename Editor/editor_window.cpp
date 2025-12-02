#include "pch.h"

#include "editor_window.h"

namespace editor
{
std::string EditorWindow::Name()
{
    return typeid(*this).name();
}

void EditorWindow::DrawGui()
{
    if (!is_open)
        return;

    if (ShouldHandleWindowAutomatically())
    {
        if (ImGui::Begin(Name().c_str(), nullptr, window_flags))
        {
            OnEditorGui();
        }

        ImGui::End();
    }
    else
    {
        OnEditorGui();
    }
}
}