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
    for (int i = 0; i < m_shared_mesh_->vertices.size(); ++i)
    {
        auto vertex = m_shared_mesh_->vertices[i];
        min_pos.x = std::min(min_pos.x, vertex.x);
        min_pos.y = std::min(min_pos.y, vertex.y);
        min_pos.z = std::min(min_pos.z, vertex.z);

        max_pos.x = max(max_pos.x, vertex.x);
        max_pos.y = max(max_pos.y, vertex.y);
        max_pos.z = max(max_pos.z, vertex.z);
    }

    DirectX::BoundingBox::CreateFromPoints(bounds, min_pos, max_pos);
}

void MeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bounds", &m_draw_bounds_);

    if (ImGui::CollapsingHeader("Mesh"))
    {
        ImGui::Indent();
        ImGui::Text("Vertices: %llu", m_shared_mesh_->vertices.size());
        ImGui::Text("Indices : %llu", m_shared_mesh_->indices.size());
        ImGui::Text("Faces(calc): %llu", !m_shared_mesh_->indices.empty() ? m_shared_mesh_->indices.size() / 3 : 0);
        ImGui::Text("UVs : %llu", m_shared_mesh_->uvs[0].size());
        ImGui::Text("UV2s: %llu", m_shared_mesh_->uvs[1].size());
        ImGui::Text("Colors  : %llu", m_shared_mesh_->colors.size());
        ImGui::Text("Normals : %llu", m_shared_mesh_->normals.size());
        ImGui::Text("Tangents: %llu", m_shared_mesh_->tangents.size());
        ImGui::Text("Bone Weights: %llu", m_shared_mesh_->bone_weights.size());
        ImGui::Text("Bind Poses  : %llu", m_shared_mesh_->bind_poses.size());
        ImGui::Text("Sub Meshes  : %llu", m_shared_mesh_->sub_meshes.size());

        if (ImGui::CollapsingHeader("Sub Meshes"))
        {
            ImGui::Indent();
            for (auto i = 0; i < m_shared_mesh_->sub_meshes.size(); i++)
            {
                if (ImGui::CollapsingHeader(("Sub Mesh " + std::to_string(i)).c_str()))
                {
                    ImGui::Indent();
                    ImGui::Text("Base Index: %d", m_shared_mesh_->sub_meshes[i].base_index);
                    ImGui::Text("Base Vert : %d", m_shared_mesh_->sub_meshes[i].base_vertex);
                    ImGui::Text("Index Count: %d", m_shared_mesh_->sub_meshes[i].index_count);
                    ImGui::Text("Vert Count : %d", m_shared_mesh_->sub_meshes[i].vertex_count);
                    ImGui::Unindent();
                }
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Materials"))
    {
        ImGui::Indent();
        for (auto i = 0; i < shared_materials.size(); ++i)
        {
            if (ImGui::CollapsingHeader(("Material " + std::to_string(i)).c_str()))
            {
                ImGui::Indent();
                ImGui::PushID(i);
                shared_materials[i].CastedLock()->OnInspectorGui();
                ImGui::PopID();
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }
}

void MeshRenderer::OnDraw()
{
    UpdateBuffers();

    auto current_material = shared_materials[0].CastedLock();
    auto current_shader = current_material->p_shared_shader.CastedLock();

    const auto cmd_list = RenderEngine::CommandList();

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

        const auto index_count = m_shared_mesh_->HasSubMeshes()
                                     ? m_shared_mesh_->sub_meshes[0].base_index
                                     : m_shared_mesh_->indices.size();

        cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), 1, 0, 0, 0);
    }

    // sub-meshes
    for (int i = 0; i < m_shared_mesh_->sub_meshes.size(); ++i)
    {
        current_material = shared_materials[i + 1].CastedLock();
        if (current_material->IsValid())
        {
            auto next_shader = current_material->p_shared_shader.CastedLock();
            if (current_shader != next_shader && next_shader != nullptr)
            {
                current_shader = next_shader;
                PSOManager::SetPipelineState(cmd_list, current_shader);
            }

            cmd_list->IASetIndexBuffer(m_index_buffers_[i + 1]->View());
            SetDescriptorTable(cmd_list, i + 1);

            const auto sub_mesh = m_shared_mesh_->sub_meshes[i];
            cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
        }
    }

    if (m_draw_bounds_)
        DrawBounds();
}

void MeshRenderer::SetSharedMesh(const std::shared_ptr<Mesh> &mesh)
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
    m_vertex_buffer_ = std::make_shared<VertexBuffer>(m_shared_mesh_.get());
    if (!m_vertex_buffer_->IsValid())
    {
        Logger::Error<MeshRenderer>("Failed to create vertex buffer!: %s", GameObject()->Name().c_str());
        buffer_creation_failed = true;
        return;
    }

    // create index buffer
    const auto index_buffer_size = m_shared_mesh_->HasSubMeshes()
                                       ? m_shared_mesh_->sub_meshes[0].base_index
                                       : m_shared_mesh_->indices.size();
    const auto indices = m_shared_mesh_->indices.data();

    auto ib = std::make_shared<IndexBuffer>(index_buffer_size * sizeof(uint32_t), indices);

    if (!ib->IsValid())
    {
        Logger::Error<MeshRenderer>("Failed to create index buffer of '%d' for '%s'", index_buffer_size,
                                    GameObject()->Path().c_str());
        buffer_creation_failed = true;
        return;
    }

    m_index_buffers_.emplace_back(ib);

    // create index buffers for sub meshes
    for (auto i = 0; i < m_shared_mesh_->sub_meshes.size(); i++)
    {
        const auto sub_mesh = m_shared_mesh_->sub_meshes[i];

        const auto sub_index_buffer_size = sub_mesh.index_count * sizeof(uint32_t);
        std::vector<uint32_t> sub_indices;
        sub_indices.insert(sub_indices.begin(),
                           m_shared_mesh_->indices.begin() + sub_mesh.base_index,
                           m_shared_mesh_->indices.begin() + sub_mesh.base_index + sub_mesh.index_count);

        const auto sub_index_buffer = std::make_shared<IndexBuffer>(sub_index_buffer_size, sub_indices.data());
        if (!sub_index_buffer->IsValid())
        {
            Logger::Error<MeshRenderer>("Failed to create sub index buffer!: sub mesh index: %d", i);
            continue;
        }

        m_index_buffers_.emplace_back(sub_index_buffer);
    }
}

void MeshRenderer::UpdateBuffers()
{
    ReconstructBuffer();
    UpdateWorldBuffer();

    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx]->GetAddress();
    const auto cmd_list = RenderEngine::CommandList();

    cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_matrix_buffer);
}

void MeshRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, const int material_idx)
{
    const auto material = shared_materials[material_idx].CastedLock();
    const auto material_block = material->p_shared_material_block;

    material->UpdateBuffer();

    for (int shader_i = 0; shader_i < kShaderType_Count; ++shader_i)
    {
        for (int param_i = 0; param_i < kParameterBufferType_Count; ++param_i)
        {
            const auto shader_type = static_cast<kShaderType>(shader_i);
            const auto param_type = static_cast<kParameterBufferType>(param_i);

            if (material_block->Empty(shader_type, param_type))
            {
                continue;
            }

            const int root_param_idx = shader_type * kParameterBufferType_Count + param_i +
                                       RootSignature::kPreDefinedVariableCount;
            const auto itr = material_block->Begin(shader_type, param_type);
            const auto desc_handle = itr->handle->HandleGPU;
            cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)