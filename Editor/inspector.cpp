#include "pch.h"

#include "inspector.h"

#include "default_editor_menus.h"
#include "imgui_stdlib.h"
#include "game_object.h"
#include "gui.h"
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

    const auto scene = std::dynamic_pointer_cast<engine::Scene>(object);
    if (scene != nullptr)
    {
        DrawScene(scene);
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

void Inspector::DrawScene(const std::shared_ptr<engine::Scene> &scene)
{
    std::string buff = scene->Name();
    if (ImGui::InputText("##INSPECTOR_GAME_OBJECT_NAME", &buff))
    {
        scene->SetName(buff);
    }

    ImGui::Separator();
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
            if (engine::Gui::ObjectHeader(component, component_name.c_str()))
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

void Inspector::DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy, const bool root)
{
    if (asset_hierarchy == nullptr)
    {
        ImGui::Text("Select a asset to inspect...");
        return;
    }

    ImGui::PushID(asset_hierarchy.get());

    {
        if (asset_hierarchy->IsFile())
            ImGui::Text("File");
        if (asset_hierarchy->IsDirectory())
            ImGui::Text("Directory");
        if (!asset_hierarchy->children.empty())
            ImGui::Text("Children: %d", asset_hierarchy->children.size());
    }

    if (asset_hierarchy->asset != nullptr)
    {
        ImGui::Separator();

        DrawAssetDescriptor(asset_hierarchy->asset);
    }

    if (asset_hierarchy->IsDirectory())
    {
        ImGui::Separator();

        if (!root || ImGui::CollapsingHeader("Show Children"))
        {
            ImGui::Indent();
            for (const auto &child : asset_hierarchy->children)
            {
                DrawAssetHierarchy(child, false);
            }
            ImGui::Unindent();
        }
    }

    ImGui::PopID();
}

void Inspector::DrawAssetDescriptor(const std::shared_ptr<engine::AssetDescriptor> &asset_descriptor)
{
    if (ImGui::CollapsingHeader("Metadata", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();

        ImGui::Text("Name: %s", asset_descriptor->AssetPath().filename().string().c_str());
        ImGui::Text("Path: %s", asset_descriptor->AssetPath().string().c_str());
        ImGui::Text("Guid: %s", asset_descriptor->Guid().str().c_str());

        const auto label = std::format("SubGuids ({0})", asset_descriptor->SubGuids().size()).c_str();
        if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();
            for (const auto &sub_guid : asset_descriptor->SubGuids())
            {
                ImGui::BulletText("%s", sub_guid.str().c_str());
            }
            ImGui::Unindent();
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();

        if (asset_descriptor->Objects().empty())
        {
            ImGui::Text("No objects found...");
        }

        for (auto &object : asset_descriptor->Objects())
        {
            ImGui::PushID(object.get());

            if (engine::Gui::ObjectHeader(object))
            {
                ImGui::Indent();

                ImGui::Text("Guid: %s", object->Guid().str().c_str());

                ImGui::Separator();

                DrawObject(object);

                ImGui::Unindent();
            }

            ImGui::PopID();
        }

        ImGui::Unindent();
    }
}
}