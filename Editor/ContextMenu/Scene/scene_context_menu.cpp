#include "pch.h"
#include "scene_context_menu.h"
#include "scene_manager.h"

namespace editor
{
bool SceneContextMenu::OnContextMenu(const std::shared_ptr<Scene> object)
{
    if (ImGui::MenuItem("Remove", nullptr, false, object != nullptr))
    {
        SceneManager::DestroyScene(object->Name());
    }

    return false;
}
}

REGISTER_CONTEXT_MENU(editor::SceneContextMenu)