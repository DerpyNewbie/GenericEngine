#include "skinned_mesh_renderer.h"

#include "dxlib_helper.h"
#include "imgui.h"
#include "Rendering/texture2d.h"

namespace engine
{
void SkinnedMeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bones", &m_draw_bones_);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Bone Info"))
    {
        for (int i = 0; i < transforms.size(); i++)
        {
            ImGui::Text("%d: %s", i, transforms[i].lock()->Name().c_str());
        }
    }

    if (ImGui::CollapsingHeader("Mesh Info"))
    {
        ImGui::Text("Vertices: %d", shared_mesh->vertices.size());
        ImGui::Text("Indices: %d", shared_mesh->indices.size());
        ImGui::Text("UVs: %d", shared_mesh->uv.size());
        ImGui::Text("UV2s: %d", shared_mesh->uv2.size());
        ImGui::Text("Colors: %d", shared_mesh->colors.size());
        ImGui::Text("Normals: %d", shared_mesh->normals.size());
    }

    ImGui::Text("Texture: %d", m_texture_.graphic_handle);
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)