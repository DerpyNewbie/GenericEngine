#include "pch.h"

#include "inspector.h"

#include "imgui_stdlib.h"
#include "game_object.h"

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

    const auto selected_obj = m_last_seen_object_.lock();

    if (selected_obj == nullptr)
    {
        ImGui::Text("Select a object to inspect...");
        return;
    }

    const auto game_object = std::dynamic_pointer_cast<engine::GameObject>(selected_obj);
    if (game_object != nullptr)
    {
        DrawGameObject(game_object);
        return;
    }

    ImGui::Text("Unknown object type is selected!");
    ImGui::Text("Object Name: '%s'", selected_obj->Name().c_str());
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
            if (ImGui::CollapsingHeader(engine::EngineUtil::GetTypeName(typeid(*component).name()).c_str(),
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                component->OnInspectorGui();
                ImGui::Unindent();
            }
            ImGui::PopID();
        }
    }
}
}