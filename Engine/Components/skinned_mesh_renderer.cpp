#include "pch.h"
#include "skinned_mesh_renderer.h"

#include "camera_component.h"
#include "Asset/asset_database.h"
#include "Rendering/gizmos.h"
#include "Components/transform.h"
#include "Rendering/gpu_resource_manager.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace
{
bool SetDescriptorTable(const std::shared_ptr<engine::MaterialBlock>& material_block)
{
    const auto resource_group = engine::GpuResourceManager::GetBuffersForMaterial(material_block);
    const auto cmd_list = RenderEngine::CommandList();

    if (!resource_group->UpdateBuffer(material_block))
        return false;
    if (!resource_group->SetBufferToDescriptorTable())
        return false;

    for (int param_i = 0; param_i < engine::kGpuBufferType_Count; ++param_i)
    {
        const auto param_type = static_cast<engine::kGpuUploadType>(param_i);

        if (resource_group->Empty(param_type))
        {
            continue;
        }

        const int root_param_idx = param_i +
            engine::RootSignature::kPreDefinedVariableCount;
        const auto itr = resource_group->Begin(param_type);
        const auto desc_handle = itr.handle->handle_gpu;
        cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
    }
    return true;
}
}

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
            m_bone_matrix_buffer_handles_[i] = DescriptorHeap::Allocate();
            m_bone_matrix_buffers_[i]->UploadBuffer(m_bone_matrix_buffer_handles_[i]);
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

    const auto& world_matrix_buffer = m_world_matrix_buffer_;
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

void SkinnedMeshRenderer::DepthRender()
{
    if (!m_cast_shadow_)
        return;

    if (m_shadow_material_ == nullptr)
    {
        m_shadow_material_ = AssetDatabase::GetAsset<Material>("ShadowMaterial/Shadow.material");
        if (m_shadow_material_ == nullptr)
        {
            Logger::Error<MeshRenderer>("ShadowMaterial is not found");
            return;
        }
    }
    
    auto cmd_list = RenderEngine::CommandList();
    auto shader = m_shadow_material_->GetShader();
    PSOManager::SetPipelineState(cmd_list, shader.CastedLock().get(), DXGI_FORMAT_R32_FLOAT, 0);

    cmd_list->IASetPrimitiveTopology(DX_PrimitiveTopology[shader->ShaderSettings().primitive_topology_type]);

    const auto mesh = m_shared_mesh_.CastedLock();
    if (mesh->vertex_buffer == nullptr)
        mesh->ReconstructMeshesBuffer();

    if (mesh->vertex_buffer)
        cmd_list->IASetVertexBuffers(0, 1, mesh->vertex_buffer->View());

    const auto world_address = m_world_matrix_buffer_->GetAddress();
    cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_address);

    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    cmd_list->SetGraphicsRootDescriptorTable(kBoneSRV, m_bone_matrix_buffer_handles_[current_buffer_idx]->handle_gpu);


    if (m_shadow_material_->shared_material_block == nullptr)
        m_shadow_material_->CreateMaterialBlock();

    if (!SetDescriptorTable(m_shadow_material_->shared_material_block))
        return;

    cmd_list->IASetIndexBuffer(mesh->index_buffers[0]->View());

    const auto index_count = mesh->HasSubMeshes()
                                 ? mesh->sub_meshes[0].base_index
                                 : mesh->indices.size();

    cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), 1, 0, 0, 0);

    // sub-meshes
    for (int i = 0; i < mesh->sub_meshes.size(); ++i)
    {
        cmd_list->IASetIndexBuffer(mesh->index_buffers[i + 1]->View());

        const auto sub_mesh = mesh->sub_meshes[i];
        cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
    }
}

void SkinnedMeshRenderer::Render()
{
    UpdateWorldBuffer();
    UpdateBoneTransformsBuffer();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();

    RenderPipeline::Submit(m_shared_mesh_.CastedLock(), shared_materials, instance_count,
                           GameObject()->Transform()->Position(), m_world_matrix_buffer_->GetAddress(),
                           m_bone_matrix_buffer_handles_[current_buffer_idx]->handle_gpu);
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)
