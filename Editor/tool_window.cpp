#include "pch.h"
#include "tool_window.h"

#include "application.h"

namespace editor
{
std::string ToolWindow::Name()
{
    return "Tools";
}

void ToolWindow::OnEditorGui()
{
    bool is_play_mode = engine::Application::Instance()->IsPlayMode();
    if (ImGui::Checkbox("Play Mode", &is_play_mode))
    {
        Editor::Instance()->SetPlayMode(is_play_mode);
    }
}
}