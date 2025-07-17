#include "pch.h"
#include "skinned_mesh_renderer.h"

#include "camera.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Components/transform.h"
#include "game_object.h"
#include "Rendering/gizmos.h"

#include <DxLib.h>

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
        bone_matrices_buffer->value = matrices;
        bone_matrices_buffer->UpdateBuffer();
    }
}

void SkinnedMeshRenderer::DrawBones()
{
    Vector3 start_pos, end_pos, sca;
    Quaternion rot;
    for (int i = 0; i < transforms.size(); ++i)
    {
        if (transforms[i].lock()->Parent())
        {
            transforms[i].lock()->WorldMatrix().Decompose(sca, rot, start_pos);
            transforms[i].lock()->Parent()->WorldMatrix().Decompose(sca, rot, end_pos);
            Gizmos::DrawLine(start_pos, end_pos);
        }
    }
}

void SkinnedMeshRenderer::UpdateWVPBuffer()
{
    WorldViewProjection wvp;
    const auto camera = Camera::Main();

    wvp.WVP[0] = GameObject()->Transform()->Parent()->WorldMatrix();
    wvp.WVP[1] = camera.lock()->GetViewMatrix();
    wvp.WVP[2] = camera.lock()->GetProjectionMatrix();

    for (auto &wvp_buffers : material_wvp_buffers)
    {
        for (size_t i = 0; i < wvp_buffers.size(); ++i)
        {
            auto wvp_buffer = wvp_buffers[i].lock();
            if (!wvp_buffer)
            {
                wvp_buffers.erase(wvp_buffers.begin() + i);
                break;
            }
            wvp_buffer->value = wvp;
            wvp_buffer->UpdateBuffer();
        }
    }
}

void SkinnedMeshRenderer::OnInspectorGui()
{
    MeshRenderer::OnInspectorGui();

    ImGui::Checkbox("Draw Bones", &m_draw_bones_);
    ImGui::Separator();
    Gui::PropertyField("Root Bone", root_bone);
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
    Matrix root_bone_matrix = GameObject()->Transform()->WorldMatrix();
    if (const auto root_bone_transform = root_bone.CastedLock())
    {
        if (const auto parent_transform = root_bone_transform->Parent())
        {
            root_bone_matrix = parent_transform->WorldMatrix();
        }
    }

    Gizmos::DrawBounds(shared_mesh->bounds, Gizmos::kDefaultColor, root_bone_matrix);
    MeshRenderer::OnDraw();
    if (m_draw_bones_)
        DrawBones();
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
        bone_transform_buffers[material_idx].lock()->value = matrices;
    }
    MeshRenderer::ReconstructMaterialBuffers(material_idx);
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    ReconstructBuffers();
    UpdateWVPBuffer();
    UpdateBoneTransformsBuffer();
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)