#include "pch.h"
#include "skinned_mesh_renderer.h"

#include "camera.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Components/transform.h"
#include "game_object.h"
#include "Rendering/gizmos.h"
#include "Rendering/view_projection.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
void SkinnedMeshRenderer::UpdateBoneTransformsBuffer()
{
    for (auto bone_matrices_buffer : m_bone_matrix_buffers_)
    {
        std::vector<Matrix> matrices(transforms.size());
        for (int i = 0; i < transforms.size(); ++i)
        {
            auto world = transforms[i].lock()->WorldMatrix();
            auto invert_bind_poses = shared_mesh->bind_poses[i].Invert();
            matrices[i] = invert_bind_poses * world;
        }
        bone_matrices_buffer->UpdateBuffer(matrices.data());
    }
}

void SkinnedMeshRenderer::UpdateWVPBuffer()
{
    ViewProjection wvp;
    const auto camera = Camera::Current();

    aaaaawvp.matrices[0] = GameObject()->Transform()->Parent()->WorldMatrix();

    *wvp_buffers[g_RenderEngine->CurrentBackBufferIndex()]->GetPtr<ViewProjection>() = wvp;
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

std::weak_ptr<Transform> SkinnedMeshRenderer::BoundsOrigin()
{
    return root_bone.CastedLock()->Parent();
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
            ImGui::Text("%d: %s", i, transforms[i].lock()->Name().c_str());
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
    UpdateBuffers();

    //draw meshes
    auto material = shared_materials[0].CastedLock();
    auto shader = material->p_shared_shader.CastedLock();
    auto cmd_list = g_RenderEngine->CommandList();
    auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();
    auto vbView = vertex_buffer->View();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);

    if (material->IsValid())
    {
        if (shader)
            PSOManager::SetPipelineState(cmd_list, shader);
        auto ibView = index_buffers[0]->View();
        cmd_list->IASetIndexBuffer(&ibView);
        cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffers[current_buffer]->GetAddress());
        cmd_list->SetGraphicsRootShaderResourceView(kBoneSRV, m_bone_matrix_buffers_[current_buffer]->GetAddress());
        SetDescriptorTable(cmd_list, 0);

        cmd_list->DrawIndexedInstanced(shared_mesh->HasSubMeshes()
                                           ? shared_mesh->sub_meshes[0].base_index
                                           : shared_mesh->indices.size(), 1, 0, 0, 0);
    }
    // sub-meshes
    for (int i = 0; i < shared_mesh->sub_meshes.size(); ++i)
    {
        material = shared_materials[i + 1].CastedLock();
        if (material->IsValid())
        {
            shader = material->p_shared_shader.CastedLock();
            cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffers[current_buffer]->GetAddress());
            if (shader)
                PSOManager::SetPipelineState(cmd_list, shader);
            auto ib = index_buffers[i + 1];
            auto sub_mesh = shared_mesh->sub_meshes[i];
            auto ibView = ib->View();
            cmd_list->IASetIndexBuffer(&ibView);
            SetDescriptorTable(cmd_list, i + 1);

            cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
        }
    }
    if (m_draw_bounds_)
        DrawBounds();
    if (m_draw_bones_)
        DrawBones();
}

void SkinnedMeshRenderer::ReconstructBuffer()
{
    MeshRenderer::ReconstructBuffer();

    for (auto &bone_matrices_buffer : m_bone_matrix_buffers_)
        if (!bone_matrices_buffer)
        {
            bone_matrices_buffer = std::make_shared<StructuredBuffer>(sizeof(Matrix), transforms.size());
            bone_matrices_buffer->CreateBuffer();
        }
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    ReconstructBuffer();
    UpdateWVPBuffer();
    UpdateBoneTransformsBuffer();
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)