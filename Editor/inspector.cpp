#include "pch.h"

#include "inspector.h"

#include "default_editor_menus.h"
#include "imgui_stdlib.h"
#include "game_object.h"
#include "Asset/asset_database.h"

namespace editor
{
std::string Inspector::Name()
{
    return "Inspector";
}
void Inspector::OnEditorGui()
{
    if (ImGui::BeginPopupContextItem("Context!"))
    {
        ImGui::MenuItem("Locked", nullptr, &m_locked_);
        ImGui::EndPopup();
    }

    if (!m_locked_)
    {
        m_last_seen_object_ = Editor::Instance()->SelectedObject();
    }

    DrawObject(m_last_seen_object_.lock());
}
void Inspector::DrawObject(const std::shared_ptr<engine::Object> &object)
{
    if (object == nullptr)
    {
        ImGui::Text("Select a object to inspect...");
        return;
    }

    const auto game_object = std::dynamic_pointer_cast<engine::GameObject>(object);
    if (game_object != nullptr)
    {
        DrawGameObject(game_object);
        return;
    }

    const auto component = std::dynamic_pointer_cast<engine::Component>(object);
    if (component != nullptr)
    {
        DrawComponent(component);
        return;
    }

    const auto inspectable = std::dynamic_pointer_cast<engine::Inspectable>(object);
    if (inspectable != nullptr)
    {
        DrawInspectable(inspectable);
        return;
    }

    const auto asset_hierarchy = std::dynamic_pointer_cast<engine::AssetHierarchy>(object);
    if (asset_hierarchy != nullptr)
    {
        DrawAssetHierarchy(asset_hierarchy);
        return;
    }

    ImGui::Text("Unknown object type is selected!");
    ImGui::Text("Object Name: '%s'", object->Name().c_str());
}
void Inspector::DrawGameObject(const std::shared_ptr<engine::GameObject> &game_object)
{
    // game object path info
    {
        std::string path = game_object->Path();
        ImGui::InputText("##INSPECTOR_GAME_OBJECT_PATH", &path, ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::Separator();

    // game object header
    {
        bool is_active_self = game_object->IsActiveSelf();
        if (ImGui::Checkbox("##INSPECTOR_GAME_OBJECT_ACTIVE", &is_active_self))
        {
            game_object->SetActive(is_active_self);
        }

        ImGui::SameLine();

        std::string buff = game_object->Name();
        if (ImGui::InputText("##INSPECTOR_GAME_OBJECT_NAME", &buff))
        {
            game_object->SetName(buff);
        }
    }

    ImGui::Separator();

    // game object components
    {
        for (const auto all_components = game_object->GetComponents();
             const auto &component : all_components)
        {
            ImGui::PushID(component.get());
            auto component_name = engine::EngineUtil::GetTypeName(typeid(*component).name());
            if (ImGui::CollapsingHeader(component_name.c_str(),
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem("##INSPECTOR_COMPONENT_POPUP"))
                {
                    if (ImGui::MenuItem("Remove", nullptr, false, component_name != "Transform"))
                    {
                        engine::Object::Destroy(component);
                    }

                    ImGui::EndPopup();
                }

                ImGui::Indent();
                DrawComponent(component);
                ImGui::Unindent();
            }
            ImGui::PopID();
        }
    }

    ImGui::Separator();
    if (ImGui::BeginPopup("##INSPECTOR_ADD_COMPONENT_POPUP"))
    {
        DefaultEditorMenu::DrawComponentMenu(game_object);
        ImGui::EndPopup();
    }

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("##INSPECTOR_ADD_COMPONENT_POPUP");
    }
}
void Inspector::DrawComponent(const std::shared_ptr<engine::Component> &component)
{
    component->OnInspectorGui();
}
void Inspector::DrawInspectable(const std::shared_ptr<engine::Inspectable> &inspectable)
{
    inspectable->OnInspectorGui();
}
void Inspector::DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy)
{
    if (asset_hierarchy->IsFile())
    {
        ImGui::Text("File: %s", asset_hierarchy->asset->path.string().c_str());
        ImGui::Text("Guid: %s", asset_hierarchy->asset->guid.str().c_str());
        ImGui::Separator();
        if (asset_hierarchy->asset->managed_object != nullptr)
        {
            DrawObject(asset_hierarchy->asset->managed_object);
        }
        else
        {
            ImGui::Text("Object is null!");
            if (ImGui::Button("Import"))
            {
                engine::AssetDatabase::GetAsset(asset_hierarchy->asset->guid);
            }
        }
        return;
    }

    ImGui::Text("Folder: %s", asset_hierarchy->asset->path.string().c_str());
}
}