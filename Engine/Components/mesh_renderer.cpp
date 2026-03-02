#include "pch.h"

#include "mesh_renderer.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"
#include "game_object.h"
#include "camera_component.h"
#include "Rendering/gizmos.h"
#include "Rendering/material_data.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
bool MeshRenderer::m_draw_bounds_ = false;

void MeshRenderer::UpdateWorldBuffer()
{
    for (auto &world_matrix_buffer : m_world_matrix_buffers_)
    {
        if (!world_matrix_buffer)
        {
            world_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            world_matrix_buffer->CreateBuffer();
        }
    }
    
    const auto world_matrix = GameObject()->Transform()->WorldMatrix();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto &world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx];
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
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
        ImGui::Unindent();
    }
}

void MeshRenderer::DepthRender()
{
    const auto cmd_list = RenderEngine::CommandList();
    const auto mesh = m_shared_mesh_.CastedLock();
    if (mesh == nullptr)
    {
        Logger::Error<MeshRenderer>("Mesh is null!");
        return;
    }

    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, mesh->vertex_buffer->View());

    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx]->GetAddress();
    cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffer);

    cmd_list->IASetIndexBuffer(mesh->index_buffers[0]->View());

    const auto index_count = mesh->HasSubMeshes()
        ? mesh->sub_meshes[0].base_index
        : mesh->indices.size();

    cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), 1, 0, 0, 0);

    // sub-meshes
    for (int i = 0; i < mesh->sub_meshes.size(); ++i)
    {
        cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffer);

        cmd_list->IASetIndexBuffer(mesh->index_buffers[i + 1]->View());

        const auto sub_mesh = mesh->sub_meshes[i];
        cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
    }
}

void MeshRenderer::Render()
{
    UpdateWorldBuffer();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();

    RenderPipeline::Submit(m_shared_mesh_.CastedLock(), shared_materials, GameObject()->Transform()->Position(), m_world_matrix_buffers_[current_buffer_idx]->GetAddress());
    RenderPipeline::SubmitRaytracing(m_shared_mesh_.CastedLock(), shared_materials, GameObject()->Transform()->WorldMatrix());
}

void MeshRenderer::SetSharedMesh(const AssetPtr<Mesh> &mesh)
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
    return GameObject()->Transform()->WorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)