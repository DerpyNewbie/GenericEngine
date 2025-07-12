#include "pch.h"
#include "skinned_mesh_renderer.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Components/transform.h"

namespace engine
{
void SkinnedMeshRenderer::ApplyBoneTransforms()
{
}

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
}

void SkinnedMeshRenderer::ReconstructMaterialBuffers(int material_idx)
{
    //set bone matrices
    bone_transform_buffers.resize(shared_materials.size());
    auto find_material_data = shared_materials[material_idx]->p_shared_material_block->
                                                              FindMaterialDataFromName("__BoneMatrices__").lock();
    if (find_material_data)
    {
        transforms.resize(shared_mesh->bind_poses.size());
        for (size_t i = 0; i < shared_mesh->bind_poses.size(); i++)
            transforms[i] = (DirectX::XMMatrixTranspose(shared_mesh->bind_poses[i]));

        auto bone_matrices_data = std::dynamic_pointer_cast<MaterialData<std::vector<Matrix>>>(find_material_data);
        bone_transform_buffers[material_idx] = bone_matrices_data;
        bone_transform_buffers[material_idx].lock()->Set(transforms);
    }
    MeshRenderer::ReconstructMaterialBuffers(material_idx);
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    MeshRenderer::UpdateBuffers();
    for (size_t i = 0; i < bone_transform_buffers.size(); ++i)
    {
        auto bone_matrices_buffer = bone_transform_buffers[i].lock();
        if (bone_matrices_buffer && shared_materials[i]->p_shared_material_block->IsCreateBuffer())
        {
            if (bone_matrices_buffer->buffer->IsValid())
            {
                bone_matrices_buffer->Set(transforms);
                bone_matrices_buffer->UpdateBuffer();
            }
        }
        else
        {
            bone_transform_buffers.erase(bone_transform_buffers.begin() + i);
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)