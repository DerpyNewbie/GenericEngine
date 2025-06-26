#include "pch.h"
#include "editor_prefs.h"

namespace editor
{
bool EditorPrefs::show_grid = true;
int EditorPrefs::theme = 0;

std::string EditorPrefs::Name()
{
    return "Editor Preferences";
}
void EditorPrefs::OnEditorGui()
{
    ImGui::Checkbox("Show Grid", &show_grid);
}

}