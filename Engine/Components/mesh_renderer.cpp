#include "pch.h"

#include "mesh_renderer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"
#include "game_object.h"
#include "camera_component.h"
#include "Asset/asset_database.h"
#include "Rendering/gizmos.h"
#include "Rendering/buffer_data_base.h"
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
bool MeshRenderer::m_draw_bounds_ = false;

void MeshRenderer::UpdateWorldBuffer()
{
    if (!m_world_matrix_buffer_)
    {
        m_world_matrix_buffer_ = std::make_shared<ConstantBuffer>(sizeof(Matrix));
        m_world_matrix_buffer_->CreateBuffer();
    }

    const auto world_matrix = GameObject()->Transform()->WorldMatrix();
    const auto ptr = m_world_matrix_buffer_->GetPtr<Matrix>();
    *ptr = world_matrix;
}

void MeshRenderer::RecalculateBoundingBox()
{
    auto min_pos = Vector3(0, 0, 0);
    auto max_pos = Vector3(0, 0, 0);
    const auto mesh = m_shared_mesh_.CastedLock();

    for (int i = 0; i < mesh->vertices.size(); ++i)
    {
        auto vertex = mesh->vertices[i];
        min_pos.x = std::min(min_pos.x, vertex.x);
        min_pos.y = std::min(min_pos.y, vertex.y);
        min_pos.z = std::min(min_pos.z, vertex.z);

        max_pos.x = std::max(max_pos.x, vertex.x);
        max_pos.y = std::max(max_pos.y, vertex.y);
        max_pos.z = std::max(max_pos.z, vertex.z);
    }

    DirectX::BoundingBox::CreateFromPoints(bounds, min_pos, max_pos);
}

void MeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bounds", &m_draw_bounds_);
    ImGui::Checkbox("Cast Shadow", &m_cast_shadow_);
    Renderer::OnInspectorGui();

    Gui::ExpandablePropertyField("Mesh", m_shared_mesh_);

    if (ImGui::CollapsingHeader("Materials"))
    {
        ImGui::Indent();
        for (auto i = 0; i < shared_materials.size(); ++i)
        {
            if (ImGui::CollapsingHeader(("Material " + std::to_string(i)).c_str()))
            {
                ImGui::Indent();
                ImGui::PushID(i);
                Gui::ExpandablePropertyField("Material" + i, shared_materials[i]);
                ImGui::PopID();
                ImGui::Unindent();
            }
        }
        Gui::PropertyField("Shadow Material", m_shadow_material_);
        ImGui::Unindent();
    }

    int instance_count_int = static_cast<int>(instance_count);
    if (Gui::PropertyField("InstanceCount", instance_count_int))
        instance_count = static_cast<uint32_t>(instance_count_int);
}

void MeshRenderer::DepthRender()
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

    if (m_shadow_material_->shared_material_block == nullptr)
        m_shadow_material_->CreateMaterialBlock();

    if (!SetDescriptorTable(m_shadow_material_->shared_material_block))
        return;

    cmd_list->IASetIndexBuffer(mesh->index_buffers[0]->View());

    const auto index_count = mesh->HasSubMeshes()
                                 ? mesh->sub_meshes[0].base_index
                                 : mesh->indices.size();

    cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), instance_count, 0, 0, 0);

    // sub-meshes
    for (int i = 0; i < mesh->sub_meshes.size(); ++i)
    {
        cmd_list->IASetIndexBuffer(mesh->index_buffers[i + 1]->View());

        const auto sub_mesh = mesh->sub_meshes[i];
        cmd_list->DrawIndexedInstanced(sub_mesh.index_count, instance_count, 0, 0, 0);
    }
}

void MeshRenderer::Render()
{
    UpdateWorldBuffer();

    RenderPipeline::Submit(m_shared_mesh_.CastedLock(), shared_materials, instance_count,
                           GameObject()->Transform()->Position(), m_world_matrix_buffer_->GetAddress());
}

AssetPtr<Material> MeshRenderer::GetShadowMaterial()
{
    return m_shadow_material_;
}

void MeshRenderer::SetSharedMesh(const AssetPtr<Mesh>& mesh)
{
    m_shared_mesh_ = mesh;
    RecalculateBoundingBox();
}

void MeshRenderer::DrawBounds()
{
    const auto matrix = BoundsOrigin();
    Gizmos::DrawBounds(bounds, Gizmos::kDefaultColor, matrix);
}

Matrix MeshRenderer::BoundsOrigin()
{
    return CameraComponent::Main()->GameObject()->Transform()->WorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)
