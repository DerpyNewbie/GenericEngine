#include "pch.h"
#include "tool_window.h"

#include "application.h"

namespace
{
enum class ButtonMode
{
    kNormal,
    kHighlighted,
    kDisabled
};

bool Button(const std::string &text, const ButtonMode mode)
{
    switch (mode)
    {
        case ButtonMode::kNormal:
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            break;
        case ButtonMode::kHighlighted:
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.8f, 1.0f));
            break;
        case ButtonMode::kDisabled:
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            break;
    }

    const auto result = ImGui::Button(text.c_str());

    ImGui::PopStyleColor();
    return result;
}
}

namespace editor
{
std::string ToolWindow::Name()
{
    return "Tools";
}

void ToolWindow::OnEditorGui()
{
    const auto editor = Editor::Instance();

    if (Button("Play", editor->GetEditorMode() == EditorMode::kPlay ? ButtonMode::kHighlighted : ButtonMode::kNormal))
    {
        editor->SetEditorMode(editor->GetEditorMode() == EditorMode::kEdit ? EditorMode::kPlay : EditorMode::kEdit);
    }

    ImGui::SameLine();

    if (Button("Pause", editor->IsPaused() ? ButtonMode::kHighlighted : ButtonMode::kNormal))
    {
        editor->SetPaused(!editor->IsPaused());
    }

    ImGui::SameLine();

    if (Button("Step", editor->GetEditorMode() == EditorMode::kPlay ? ButtonMode::kNormal : ButtonMode::kDisabled))
    {
        editor->SingleTickStep();
    }
}
}