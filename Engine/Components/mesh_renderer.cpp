#include "pch.h"

#include "mesh_renderer.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"
#include "game_object.h"
#include "camera_component.h"
#include "Rendering/gizmos.h"
#include "Rendering/material_data.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
bool MeshRenderer::m_draw_bounds_ = false;

void MeshRenderer::UpdateWorldBuffer()
{
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto &world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx];
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
    *ptr = WorldMatrix();
}

void MeshRenderer::RecalculateBoundingBox()
{
    auto min_pos = Vector3(0, 0, 0);
    auto max_pos = Vector3(0, 0, 0);
    const auto mesh = m_shared_mesh_.CastedLock();

    if (mesh == nullptr)
        return;

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

void MeshRenderer::OnAwake()
{
    Renderer::OnAwake();
    RecalculateBoundingBox();
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
void MeshRenderer::UpdateBuffer()
{
    ReconstructBuffer();
    UpdateWorldBuffer();
}

void MeshRenderer::Render()
{
    // TODO: fallback to error material
    if (shared_materials.empty())
    {
        Logger::Error<MeshRenderer>("No materials assigned!");
        return;
    }

    auto current_material = shared_materials[0].CastedLock();
    if (current_material == nullptr)
        return;

    auto current_shader = current_material->p_shared_shader.CastedLock();
    if (current_shader == nullptr)
        return;

    const auto cmd_list = RenderEngine::CommandList();
    const auto mesh = m_shared_mesh_.CastedLock();
    if (mesh == nullptr)
    {
        Logger::Error<MeshRenderer>("Mesh is null!");
        return;
    }

    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, m_vertex_buffer_->View());

    if (current_material->IsValid())
    {
        if (current_shader)
        {
            PSOManager::SetPipelineState(cmd_list, current_shader);
        }

        cmd_list->IASetIndexBuffer(m_index_buffers_[0]->View());
        SetDescriptorTable(cmd_list, 0);
        const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
        const auto world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx]->GetAddress();

        cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffer);
        const auto index_count = mesh->HasSubMeshes()
            ? mesh->sub_meshes[0].base_index
            : mesh->indices.size();

        cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), 1, 0, 0, 0);
    }

    // sub-meshes
    for (int i = 0; i < mesh->sub_meshes.size(); ++i)
    {
        current_material = shared_materials[i + 1].CastedLock();
        if (current_material != nullptr && current_material->IsValid())
        {
            auto next_shader = current_material->p_shared_shader.CastedLock();
            if (current_shader != next_shader && next_shader != nullptr)
            {
                current_shader = next_shader;
                PSOManager::SetPipelineState(cmd_list, current_shader);
            }

            cmd_list->IASetIndexBuffer(m_index_buffers_[i + 1]->View());
            SetDescriptorTable(cmd_list, i + 1);

            const auto sub_mesh = mesh->sub_meshes[i];
            cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
        }
    }

    if (m_draw_bounds_)
        DrawBounds();
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
    cmd_list->IASetVertexBuffers(0, 1, m_vertex_buffer_->View());

    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx]->GetAddress();
    cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffer);

    cmd_list->IASetIndexBuffer(m_index_buffers_[0]->View());

    const auto index_count = mesh->HasSubMeshes()
        ? mesh->sub_meshes[0].base_index
        : mesh->indices.size();

    cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), 1, 0, 0, 0);

    // sub-meshes
    for (int i = 0; i < mesh->sub_meshes.size(); ++i)
    {
        cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffer);

        cmd_list->IASetIndexBuffer(m_index_buffers_[i + 1]->View());

        const auto sub_mesh = mesh->sub_meshes[i];
        cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
    }
}

void MeshRenderer::SetSharedMesh(const AssetPtr<Mesh> &mesh)
{
    m_shared_mesh_ = mesh;
    RecalculateBoundingBox();
}

void MeshRenderer::DrawBounds()
{
    const auto matrix = BoundsOrigin()->WorldMatrix();
    Gizmos::DrawBounds(bounds, Gizmos::kDefaultColor, matrix);
}

std::shared_ptr<Transform> MeshRenderer::BoundsOrigin()
{
    return GameObject()->Transform();
}

void MeshRenderer::ReconstructBuffer()
{
    if (!m_vertex_buffer_ || m_index_buffers_.empty())
    {
        if (buffer_creation_failed)
        {
            return;
        }
        ReconstructMeshesBuffer();
    }

    for (auto &world_matrix_buffer : m_world_matrix_buffers_)
    {
        if (!world_matrix_buffer)
        {
            world_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            world_matrix_buffer->CreateBuffer();
        }
    }
}

Matrix MeshRenderer::WorldMatrix()
{
    return GameObject()->Transform()->WorldMatrix();
}

void MeshRenderer::ReconstructMeshesBuffer()
{
    // clean up old buffers
    if (m_vertex_buffer_)
    {
        m_vertex_buffer_ = nullptr;
    }

    if (!m_index_buffers_.empty())
    {
        for (auto &index_buffer : m_index_buffers_)
            index_buffer = nullptr;
        m_index_buffers_.clear();
    }

    // create vertex buffer
    const auto mesh = m_shared_mesh_.CastedLock();
    m_vertex_buffer_ = std::make_shared<VertexBuffer>(mesh.get());
    if (!m_vertex_buffer_->IsValid())
    {
        Logger::Error<MeshRenderer>("Failed to create vertex buffer!: %s", GameObject()->Name().c_str());
        buffer_creation_failed = true;
        return;
    }

    // create index buffer
    const auto index_buffer_size = mesh->HasSubMeshes()
        ? mesh->sub_meshes[0].base_index
        : mesh->indices.size();
    const auto indices = mesh->indices.data();

    auto ib = std::make_shared<IndexBuffer>(index_buffer_size * sizeof(uint32_t), indices);

    if (!ib->IsValid())
    {
        Logger::Error<MeshRenderer>(
            "Failed to create index buffer of '%d' for '%s'",
            index_buffer_size,
            GameObject()->Path().c_str()
        );
        buffer_creation_failed = true;
        return;
    }

    m_index_buffers_.emplace_back(ib);

    // create index buffers for sub meshes
    for (auto i = 0; i < mesh->sub_meshes.size(); i++)
    {
        const auto sub_mesh = mesh->sub_meshes[i];

        const auto sub_index_buffer_size = sub_mesh.index_count * sizeof(uint32_t);
        std::vector<uint32_t> sub_indices;
        sub_indices.insert(
            sub_indices.begin(),
            mesh->indices.begin() + sub_mesh.base_index,
            mesh->indices.begin() + sub_mesh.base_index + sub_mesh.index_count
        );

        const auto sub_index_buffer = std::make_shared<IndexBuffer>(sub_index_buffer_size, sub_indices.data());
        if (!sub_index_buffer->IsValid())
        {
            Logger::Error<MeshRenderer>("Failed to create sub index buffer!: sub mesh index: %d", i);
            continue;
        }

        m_index_buffers_.emplace_back(sub_index_buffer);
    }
}

void MeshRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, const int material_idx)
{
    const auto material = shared_materials[material_idx].CastedLock();

    material->SetDescriptorTable();
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)