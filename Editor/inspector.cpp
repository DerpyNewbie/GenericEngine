#include "pch.h"

#include "inspector.h"

#include "default_editor_menus.h"
#include "editor_prefs.h"
#include "imgui_stdlib.h"
#include "game_object.h"
#include "gui.h"
#include "Asset/asset_database.h"

namespace editor
{
using namespace engine;

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

void Inspector::DrawObject(const std::shared_ptr<Object> &object)
{
    if (object == nullptr)
    {
        ImGui::Text("Select a object to inspect...");
        return;
    }

    if (EditorPrefs::show_editor_debug)
    {
        Gui::ReadOnlyStringField("Guid", object->Guid().str());
    }

    const auto scene = std::dynamic_pointer_cast<Scene>(object);
    if (scene != nullptr)
    {
        DrawScene(scene);
        return;
    }

    const auto game_object = std::dynamic_pointer_cast<GameObject>(object);
    if (game_object != nullptr)
    {
        DrawGameObject(game_object);
        return;
    }

    const auto component = std::dynamic_pointer_cast<Component>(object);
    if (component != nullptr)
    {
        DrawComponent(component);
        return;
    }

    const auto inspectable = std::dynamic_pointer_cast<Inspectable>(object);
    if (inspectable != nullptr)
    {
        DrawInspectable(inspectable);
        return;
    }

    const auto asset_hierarchy = std::dynamic_pointer_cast<AssetHierarchy>(object);
    if (asset_hierarchy != nullptr)
    {
        DrawAssetHierarchy(asset_hierarchy);
        return;
    }

    ImGui::Text("Unknown object type is selected!");
    ImGui::Text("Object Name: '%s'", object->Name().c_str());
}

void Inspector::DrawScene(const std::shared_ptr<Scene> &scene)
{
    std::string buff = scene->Name();
    if (ImGui::InputText("##INSPECTOR_GAME_OBJECT_NAME", &buff))
    {
        scene->SetName(buff);
    }

    ImGui::Separator();
}

void Inspector::DrawGameObject(const std::shared_ptr<GameObject> &game_object)
{
    if (EditorPrefs::show_editor_debug)
    {
        Gui::ReadOnlyStringField("Scene", game_object->Scene() ? game_object->Scene()->Name() : "!!!NULL!!!");
        Gui::ReadOnlyStringField("Path", game_object->Path());
    }

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
            auto component_name = EngineUtil::GetTypeName(typeid(*component).name());
            if (Gui::ObjectHeader(component, component_name.c_str()))
            {
                if (ImGui::BeginPopupContextItem("##INSPECTOR_COMPONENT_POPUP"))
                {
                    if (ImGui::MenuItem("Remove", nullptr, false, component_name != "Transform"))
                    {
                        Object::Destroy(component);
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

void Inspector::DrawComponent(const std::shared_ptr<Component> &component)
{
    component->OnInspectorGui();
}

void Inspector::DrawInspectable(const std::shared_ptr<Inspectable> &inspectable)
{
    inspectable->OnInspectorGui();
}

void Inspector::DrawAssetHierarchy(const std::shared_ptr<AssetHierarchy> &asset_hierarchy, const bool root)
{
    if (asset_hierarchy == nullptr)
    {
        ImGui::Text("Select a asset to inspect...");
        return;
    }

    ImGui::PushID(asset_hierarchy.get());
    if (ImGui::CollapsingHeader(asset_hierarchy->Name().c_str(), root ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None))
    {
        ImGui::Indent();
        if (EditorPrefs::show_editor_debug)
        {
            bool debug_info_shown = false;
            if (asset_hierarchy->IsFile())
            {
                ImGui::Text("File");
                debug_info_shown = true;
            }
            if (asset_hierarchy->IsDirectory())
            {
                ImGui::Text("Directory");
                debug_info_shown = true;
            }
            if (!asset_hierarchy->children.empty())
            {
                ImGui::Text("Children: %d", asset_hierarchy->children.size());
                debug_info_shown = true;
            }

            if (debug_info_shown)
            {
                ImGui::Separator();
            }
        }

        if (asset_hierarchy->asset != nullptr)
        {
            DrawAssetDescriptor(asset_hierarchy->asset);
        }

        if (asset_hierarchy->IsDirectory())
        {
            ImGui::Indent();
            for (const auto &child : asset_hierarchy->children)
            {
                DrawAssetHierarchy(child, false);
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }
    ImGui::PopID();
}

void Inspector::DrawAssetDescriptor(const std::shared_ptr<AssetDescriptor> &asset_descriptor)
{
    if (EditorPrefs::show_editor_debug && ImGui::CollapsingHeader("Metadata", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();

        ImGui::Text("Name: %s", asset_descriptor->AssetPath().filename().string().c_str());
        ImGui::Text("Path: %s", asset_descriptor->AssetPath().string().c_str());
        ImGui::Text("Guid: %s", asset_descriptor->Guid().str().c_str());

        const std::string label = std::format("SubGuids ({})", asset_descriptor->SubGuids().size());
        if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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

    const auto main_object = asset_descriptor->MainObject();
    if (main_object != nullptr)
    {
        ImGui::PushID(main_object.get());
        DrawObject(main_object);
        ImGui::PopID();
    }

    if (asset_descriptor->Objects().size() > 1 && ImGui::CollapsingHeader("Objects"))
    {
        ImGui::Indent();
        for (auto &object : asset_descriptor->Objects())
        {
            if (object == main_object)
                continue;

            ImGui::PushID(object.get());

            if (Gui::ObjectHeader(object))
            {
                ImGui::Indent();

                if (EditorPrefs::show_editor_debug)
                {
                    ImGui::Text("Guid: %s", object->Guid().str().c_str());
                    ImGui::Separator();
                }

                DrawObject(object);

                ImGui::Unindent();
            }

            ImGui::PopID();
        }
        ImGui::Unindent();
    }
}
}