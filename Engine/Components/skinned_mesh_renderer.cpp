#include "pch.h"
#include "skinned_mesh_renderer.h"

#include "camera_component.h"
#include "Rendering/gizmos.h"
#include "Components/transform.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
bool SkinnedMeshRenderer::m_draw_bones_ = false;

void SkinnedMeshRenderer::UpdateBoneTransformsBuffer()
{
    for (int i = 0; i < m_bone_matrix_buffers_.size(); ++i)
    {
        if (!m_bone_matrix_buffers_[i])
        {
            m_bone_matrix_buffers_[i] = std::make_shared<StructuredBuffer>(sizeof(Matrix), transforms.size());
            m_bone_matrix_buffers_[i]->CreateBuffer();
            m_bone_matrix_buffer_handles_[i] = m_bone_matrix_buffers_[i]->UploadBuffer();
        }
    }

    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
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

void SkinnedMeshRenderer::UpdateWorldBuffer()
{
    if (!m_world_matrix_buffer_)
    {
        m_world_matrix_buffer_ = std::make_shared<ConstantBuffer>(sizeof(Matrix));
        m_world_matrix_buffer_->CreateBuffer();
    }

    const auto &world_matrix_buffer = m_world_matrix_buffer_;
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
    *ptr = Matrix::Identity;
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

Matrix SkinnedMeshRenderer::BoundsOrigin()
{
    if (const auto root = root_bone.CastedLock())
    {
        return root->Parent()->WorldMatrix();
    }

    return GameObject()->Transform()->WorldMatrix();
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
void SkinnedMeshRenderer::UpdateBuffer()
{
    MeshRenderer::UpdateBuffer();
    UpdateBoneTransformsBuffer();

    const auto current_buffer = RenderEngine::CurrentBackBufferIndex();
    const auto cmd_list = RenderEngine::CommandList();

    cmd_list->SetGraphicsRootDescriptorTable(kBoneSRV, m_bone_matrix_buffer_handles_[current_buffer]->handle_gpu);
}

void SkinnedMeshRenderer::Render()
{
    UpdateWorldBuffer();
    UpdateBoneTransformsBuffer();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();

    RenderPipeline::Submit(m_shared_mesh_.CastedLock(), shared_materials, m_instance_count_, GameObject()->Transform()->Position(), m_world_matrix_buffer_->GetAddress(), m_bone_matrix_buffer_handles_[current_buffer_idx]->handle_gpu);
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)