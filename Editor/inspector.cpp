#include "inspector.h"

#include "imgui.h"
#include "../Engine/game_object.h"

namespace editor
{
void Inspector::OnConstructed()
{
    IEditorWindow::OnConstructed();
    SetName("Inspector");
}
void Inspector::OnEditorGui()
{

    if (selected_game_object_ptr == nullptr)
    {
        ImGui::Text("Inspector is uninitialized!");
        return;
    }

    std::shared_ptr<engine::GameObject> selected_go = *selected_game_object_ptr;
    if (selected_go == nullptr)
    {
        ImGui::Text("Select game object to inspect...");
        return;
    }

    for (const auto all_components = selected_go->GetComponents<engine::Component>();
         const auto& component : all_components)
    {
        if (ImGui::CollapsingHeader(engine::EngineUtil::GetTypeName(typeid(*component).name()).c_str()))
        {
            ImGui::Text("TODO: Draw some fields!");
        }
    }
}
}