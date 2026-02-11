#include "pch.h"
#include "render_pass.h"

#include "gui.h"

CEREAL_REGISTER_TYPE(engine::RenderPass)

void engine::RenderPass::OnInspectorGui()
{
    for (int i = 0; i < shaders.size(); ++i)
    {
        ImGui::PushID(i);
        Gui::ExpandablePropertyField("shader", shaders[i]);
        ImGui::PopID();
    }
    
    if (ImGui::Button("Add"))
    {
        shaders.emplace_back();
    }
    if (ImGui::Button("Remove"))
    {
        shaders.erase(shaders.end() - 1);
    }
}