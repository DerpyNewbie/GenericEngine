#include "pch.h"
#include "game_object_context_menu.h"
#include "game_object_create_menu.h"
#include "serializer.h"
#include "Editor/editor_prefs.h"
#include "Editor/ContextMenu/Common/add_component_menu.h"

namespace editor
{
bool GameObjectContextMenu::OnContextMenu(std::shared_ptr<GameObject> object)
{
    if (ImGui::BeginMenu("Create"))
    {
        GameObjectCreateMenu::Draw(object);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Add Component", object != nullptr))
    {
        AddComponentMenu::Draw(object);
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Duplicate", nullptr, false, object != nullptr))
    {
        if (const auto cloned_object = Object::Instantiate(object))
        {
            Logger::Log<GameObjectContextMenu>("Cloned %s", cloned_object->Name().c_str());
        }
        else
        {
            Logger::Error<GameObjectContextMenu>("Failed to clone object. Check logs for more details");
        }
    }

    if (ImGui::MenuItem("Delete", nullptr, false, object != nullptr))
    {
        Object::DestroyImmediate(object);
    }

    if (EditorPrefs::show_editor_debug)
    {
        if (ImGui::MenuItem("Debug: Inspect"))
        {
            std::stringstream ss;
            {
                Serializer serializer;
                auto _ = serializer.Save(ss, object);
            }

            const std::string serialized_object(ss.view());
            Logger::Log(serialized_object.c_str());
        }
    }

    return false;
}
}