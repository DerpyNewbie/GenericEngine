#include "pch.h"
#include "skinned_mesh_renderer.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Components/transform.h"

namespace engine
{
void SkinnedMeshRenderer::UpdateBoneTransformsBuffer()
{
    for (size_t i = 0; i < bone_transform_buffers.size(); ++i)
    {
        auto bone_matrices_buffer = bone_transform_buffers[i].lock();
        if (!bone_matrices_buffer)
        {
            bone_transform_buffers.erase(bone_transform_buffers.begin() + i);
            break;
        }
        std::vector<Matrix> matrices(transforms.size());
        for (int i = 0; i < transforms.size(); ++i)
        {
            auto world = transforms[i].lock()->WorldMatrix();
            auto invert_bind_poses = shared_mesh->bind_poses[i].Invert();
            matrices[i] = invert_bind_poses * world;
        }
        bone_matrices_buffer->Set(matrices);
        bone_matrices_buffer->UpdateBuffer();
    }
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
        auto bone_matrices_data = std::dynamic_pointer_cast<MaterialData<std::vector<Matrix>>>(find_material_data);
        bone_transform_buffers[material_idx] = bone_matrices_data;
        std::vector<Matrix> matrices(transforms.size());
        //ここで一回セットしておかないとCreateBufferでうまくいかない
        for (int i = 0; i < transforms.size(); ++i)
        {
            auto world = transforms[i].lock()->WorldMatrix();
            auto invert_bind_poses = shared_mesh->bind_poses[i].Invert();
            matrices[i] = invert_bind_poses * world;
        }
        bone_transform_buffers[material_idx].lock()->Set(matrices);
    }
    MeshRenderer::ReconstructMaterialBuffers(material_idx);
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    MeshRenderer::UpdateBuffers();
    UpdateBoneTransformsBuffer();
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)