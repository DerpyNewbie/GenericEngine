#include "pch.h"
#include "editor_prefs.h"

namespace editor
{
bool EditorPrefs::show_editor_debug = true;
bool EditorPrefs::show_grid = true;
bool EditorPrefs::show_physics_debug = true;
int EditorPrefs::theme = 0;

std::string EditorPrefs::Name()
{
    return "Editor Preferences";
}
void EditorPrefs::OnEditorGui()
{
    ImGui::Checkbox("Show Editor Debug", &show_editor_debug);
    ImGui::Checkbox("Show Grid", &show_grid);
    ImGui::Checkbox("Show Physics Debug", &show_physics_debug);
    ImGui::Combo("Theme", &EditorPrefs::theme, "Dark\0Light\0Classic\0\0");
}

}