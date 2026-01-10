#include "pch.h"
#include "component_context_menu.h"

#include "Editor/ContextMenu/Common/add_component_menu.h"

namespace editor
{
bool ComponentContextMenu::OnContextMenu(const std::shared_ptr<Component> component)
{
    if (ImGui::MenuItem("Remove", nullptr, false, component != nullptr))
    {
        Object::DestroyImmediate(component);
    }

    if (ImGui::MenuItem("Add Component", nullptr, false, component != nullptr && component->GameObject() != nullptr))
    {
        AddComponentMenu::Draw(component->GameObject());
    }

    return false;
}
}

REGISTER_CONTEXT_MENU(editor::ComponentContextMenu)