#include "pch.h"

#include "inspector.h"

#include "imgui_stdlib.h"
#include "game_object.h"

namespace editor
{
Inspector::Inspector(std::shared_ptr<engine::GameObject> *selected_obj_ptr) : selected_game_object_ptr(selected_obj_ptr)
{}
std::string Inspector::Name()
{
    return "Inspector";
}
void Inspector::OnEditorGui()
{
    if (selected_game_object_ptr == nullptr)
    {
        ImGui::Text("Inspector is uninitialized!");
        return;
    }

    const std::shared_ptr<engine::GameObject> selected_go = *selected_game_object_ptr;
    if (selected_go == nullptr)
    {
        ImGui::Text("Select game object to inspect...");
        return;
    }

    // game object path info
    {
        std::string path = selected_go->Path();
        ImGui::InputText("##INSPECTOR_GAME_OBJECT_PATH", &path, ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::Separator();

    // game object header
    {
        bool is_active_self = selected_go->IsActiveSelf();
        if (ImGui::Checkbox("##INSPECTOR_GAME_OBJECT_ACTIVE", &is_active_self))
        {
            selected_go->SetActive(is_active_self);
        }

        ImGui::SameLine();

        std::string buff = selected_go->Name();
        if (ImGui::InputText("##INSPECTOR_GAME_OBJECT_NAME", &buff))
        {
            selected_go->SetName(buff);
        }
    }

    ImGui::Separator();

    // game object components
    {
        for (const auto all_components = selected_go->GetComponents();
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