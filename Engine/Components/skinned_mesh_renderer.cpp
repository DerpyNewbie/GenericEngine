#include "pch.h"

#include "skinned_mesh_renderer.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

namespace engine
{
void SkinnedMeshRenderer::OnInspectorGui()
{
    MeshRenderer::OnInspectorGui();

    ImGui::Checkbox("Draw Bones", &m_draw_bones_);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Bone Info"))
    {
        for (int i = 0; i < transforms.size(); i++)
        {
            //ImGui::Text("%d: %s", i, transforms[i].lock()->Name().c_str());
        }
    }

    if (ImGui::CollapsingHeader("Mesh Info"))
    {
        ImGui::Text("Vertices: %d", shared_mesh->vertices.size());
        ImGui::Text("Indices: %d", shared_mesh->indices.size());
        ImGui::Text("UVs: %d", shared_mesh->uvs[0].size());
        ImGui::Text("UV2s: %d", shared_mesh->uvs[1].size());
        ImGui::Text("Colors: %d", shared_mesh->colors.size());
        ImGui::Text("Normals: %d", shared_mesh->normals.size());
    }
}

void SkinnedMeshRenderer::OnDraw()
{
    MeshRenderer::OnDraw();
}

void SkinnedMeshRenderer::ReconstructBuffers()
{
    MeshRenderer::ReconstructBuffers();

    transforms.reserve(shared_mesh->bind_poses.size());
    for (size_t i = 0; i < shared_mesh->bind_poses.size(); i++)
        transforms.emplace_back(shared_mesh->bind_poses[i]);
}

void SkinnedMeshRenderer::ReconstructMaterialBuffers(int material_idx)
{
    MeshRenderer::ReconstructMaterialBuffers(material_idx);
    auto find_material_data = shared_materials[material_idx]->p_shared_material_block.CastedLock()->
                                                              FindMaterialDataFromName("BoneMatrices").lock();
    if (find_material_data)
    {
        auto bone_matrices_data = std::dynamic_pointer_cast<MaterialData<std::vector<Matrix>>>(find_material_data);
        material_bone_matrices_buffers[material_idx] = bone_matrices_data;
    }
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    MeshRenderer::UpdateBuffers();
    for (auto bone_matrices_buffer: material_bone_matrices_buffers)
    {
        bone_matrices_buffer.lock()->Set(transforms);
    }
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)