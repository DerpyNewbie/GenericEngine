#include "pch.h"
#include "skinned_mesh_renderer.h"

#include "camera_component.h"
#include "Rendering/gizmos.h"
#include "Components/transform.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
bool SkinnedMeshRenderer::m_draw_bones_ = false;

void SkinnedMeshRenderer::UpdateBoneTransformsBuffer() const
{
    const auto current_buffer_idx = g_RenderEngine->CurrentBackBufferIndex();
    const auto bone_matrices_buffer = m_bone_matrix_buffers_[current_buffer_idx];

    std::vector<Matrix> matrices(transforms.size());
    for (int i = 0; i < transforms.size(); ++i)
    {
        auto world = transforms[i].lock()->WorldMatrix();
        auto invert_bind_poses = inverted_bind_poses[i];
        matrices[i] = invert_bind_poses * world;
    }

    bone_matrices_buffer->UpdateBuffer(matrices.data());
}

Matrix SkinnedMeshRenderer::WorldMatrix()
{
    return Matrix::Identity;
}

void SkinnedMeshRenderer::DrawBones() const
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

std::shared_ptr<Transform> SkinnedMeshRenderer::BoundsOrigin()
{
    return root_bone.CastedLock()->Parent();
}

void SkinnedMeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bones", &m_draw_bones_);

    MeshRenderer::OnInspectorGui();

    Gui::PropertyField("Root Bone", root_bone);
    if (ImGui::CollapsingHeader("Bone Info"))
    {
        for (int i = 0; i < transforms.size(); i++)
        {
            ImGui::Text("%d: %s", i, transforms[i].lock()->Name().c_str());
        }
    }
}

void SkinnedMeshRenderer::OnDraw()
{
    UpdateBuffers();

    MeshRenderer::OnDraw();

    if (m_draw_bones_)
        DrawBones();
}

void SkinnedMeshRenderer::ReconstructBuffer()
{
    MeshRenderer::ReconstructBuffer();

    for (auto &bone_matrices_buffer : m_bone_matrix_buffers_)
    {
        if (!bone_matrices_buffer)
        {
            bone_matrices_buffer = std::make_shared<StructuredBuffer>(sizeof(Matrix), transforms.size());
            bone_matrices_buffer->CreateBuffer();
        }
    }
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    MeshRenderer::UpdateBuffers();
    UpdateBoneTransformsBuffer();

    const auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();
    const auto bone_matrix_buffer = m_bone_matrix_buffers_[current_buffer]->GetAddress();
    const auto cmd_list = g_RenderEngine->CommandList();

    cmd_list->SetGraphicsRootShaderResourceView(kBoneSRV, bone_matrix_buffer);
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)