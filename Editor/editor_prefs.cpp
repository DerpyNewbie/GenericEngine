#include "pch.h"
#include "editor_prefs.h"

namespace editor
{
bool EditorPrefs::show_grid = true;
int EditorPrefs::theme = 0;
void EditorPrefs::OnConstructed()
{
    IEditorWindow::OnConstructed();
    SetName("Editor Preferences");
}
void EditorPrefs::OnEditorGui()
{
    ImGui::Checkbox("Show Grid", &show_grid);
}

}