#include "pch.h"
#include "scene_context_menu.h"
#include "scene_manager.h"

namespace editor
{
bool SceneContextMenu::OnContextMenu(std::shared_ptr<Scene> object)
{
    if (ImGui::MenuItem("Remove", nullptr, false, object != nullptr))
    {
        SceneManager::DestroyScene(object->Name());
    }

    return false;
}
}