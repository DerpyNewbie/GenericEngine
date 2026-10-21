#include "pch.h"
#include "skinned_mesh_renderer.h"
#include "game_object.h"
#include "gui.h"
#include "Rendering/gizmos.h"
#include "Components/transform.h"
#include "Rendering/DirectX/render_engine.h"
#include "Rendering/CaboRenderingSystem/render_engine.h"

namespace engine
{
bool SkinnedMeshRenderer::m_draw_bones_ = false;

void SkinnedMeshRenderer::UpdateBoneTransformsBuffer()
{
    if (transforms.empty())
        return;

    if (!m_bone_matrix_buffer_.IsValid() || m_bone_matrix_buffer_.Count() != transforms.size())
    {
        m_bone_matrix_buffer_ = CRS::StructuredBuffer<Matrix>(
            RenderEngine::NewRenderer().Device(), static_cast<uint32_t>(transforms.size()), CRS::MemoryType::Upload,
            CRS::BufferUsage::None, "BoneMatrices");
    }

    std::vector<Matrix> matrices(transforms.size());
    for (int i = 0; i < transforms.size(); ++i)
    {
        const auto transform = transforms[i].lock();
        const auto world = transform ? transform->WorldMatrix() : Matrix::Identity;
        matrices[i] = inverted_bind_poses[i] * world;
    }

    m_bone_matrix_buffer_.Set(matrices);
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
    if (root_bone != nullptr)
    {
        return root_bone->Parent()->WorldMatrix();
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

void SkinnedMeshRenderer::Render()
{
    UpdateBoneTransformsBuffer();

    if (m_draw_bones_)
        DrawBones();

    // ボーン行列がworld空間なのでWorldMatrixは単位行列。カリング用のboundsはworld空間に直して渡す
    DirectX::BoundingBox world_bounds;
    bounds.Transform(world_bounds, BoundsOrigin());
    SubmitToRenderer(Matrix::Identity, world_bounds, m_bone_matrix_buffer_.GetHandle());
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)
