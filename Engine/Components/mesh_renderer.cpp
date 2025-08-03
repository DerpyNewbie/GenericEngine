#include "pch.h"

#include "mesh_renderer.h"
#include "DxLib/dxlib_helper.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"
#include "game_object.h"
#include "camera.h"
#include "Rendering/material_data.h"
#include "Rendering/view_projection.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
void MeshRenderer::UpdateWVPBuffer()
{
    *wvp_buffers[g_RenderEngine->CurrentBackBufferIndex()]->GetPtr<Matrix>() = GameObject()->Transform()->
        WorldMatrix();
}

void MeshRenderer::RecalculateBoundingBox()
{
    auto min_pos = Vector3(0, 0, 0);
    auto max_pos = Vector3(0, 0, 0);
    for (int i = 0; i < shared_mesh->vertices.size(); ++i)
    {
        auto vertex = shared_mesh->vertices[i];
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
    if (ImGui::CollapsingHeader("Mesh"))
    {
        ImGui::Indent();
        ImGui::Text("Vertices: %llu", shared_mesh->vertices.size());
        ImGui::Text("Indices : %llu", shared_mesh->indices.size());
        ImGui::Text("Faces(calc): %llu", !shared_mesh->indices.empty() ? shared_mesh->indices.size() / 3 : 0);
        ImGui::Text("UVs : %llu", shared_mesh->uvs[0].size());
        ImGui::Text("UV2s: %llu", shared_mesh->uvs[1].size());
        ImGui::Text("Colors  : %llu", shared_mesh->colors.size());
        ImGui::Text("Normals : %llu", shared_mesh->normals.size());
        ImGui::Text("Tangents: %llu", shared_mesh->tangents.size());
        ImGui::Text("Bone Weights: %llu", shared_mesh->bone_weights.size());
        ImGui::Text("Bind Poses  : %llu", shared_mesh->bind_poses.size());
        ImGui::Text("Sub Meshes  : %llu", shared_mesh->sub_meshes.size());
        if (ImGui::CollapsingHeader("Sub Meshes"))
        {
            ImGui::Indent();
            for (auto i = 0; i < shared_mesh->sub_meshes.size(); i++)
            {
                if (ImGui::CollapsingHeader(("Sub Mesh " + std::to_string(i)).c_str()))
                {
                    ImGui::Indent();
                    ImGui::Text("Base Index: %d", shared_mesh->sub_meshes[i].base_index);
                    ImGui::Text("Base Vert : %d", shared_mesh->sub_meshes[i].base_vertex);
                    ImGui::Text("Index Count: %d", shared_mesh->sub_meshes[i].index_count);
                    ImGui::Text("Vert Count : %d", shared_mesh->sub_meshes[i].vertex_count);
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
    ImGui::Checkbox("Draw Bounds", &m_draw_bounds_);
}

void MeshRenderer::OnDraw()
{
    UpdateBuffers();

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
}

void MeshRenderer::SetSharedMesh(std::shared_ptr<Mesh> mesh)
{
    shared_mesh = mesh;
    RecalculateBoundingBox();
}

void MeshRenderer::DrawBounds()
{
    auto matrix = BoundsOrigin().lock()->WorldMatrix();
    Gizmos::DrawBounds(bounds, Gizmos::kDefaultColor, matrix);
}

std::weak_ptr<Transform> MeshRenderer::BoundsOrigin()
{
    return GameObject()->Transform();
}

void MeshRenderer::ReconstructBuffer()
{
    if (!vertex_buffer || index_buffers.empty())
    {
        if (buffer_creation_failed)
        {
            return;
        }
        ReconstructMeshesBuffer();
    }

    for (auto &world_matrix_buffer : world_matrix_buffers)
    {
        if (!world_matrix_buffer)
        {
            world_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            world_matrix_buffer->CreateBuffer();
        }
    }
}

void MeshRenderer::ReconstructMeshesBuffer()
{
    // clean up old buffers
    if (vertex_buffer)
    {
        vertex_buffer = nullptr;
    }

    if (!index_buffers.empty())
    {
        for (auto &index_buffer : index_buffers)
            index_buffer = nullptr;
        index_buffers.clear();
    }

    // create vertex buffer
    vertex_buffer = std::make_shared<VertexBuffer>(shared_mesh.get());
    if (!vertex_buffer->IsValid())
    {
        Logger::Error<MeshRenderer>("Failed to create vertex buffer!: %s", GameObject()->Name().c_str());
        buffer_creation_failed = true;
        return;
    }

    // create index buffer
    const auto ib_size = shared_mesh->HasSubMeshes()
                             ? shared_mesh->sub_meshes[0].base_index
                             : shared_mesh->indices.size();
    const auto indices = shared_mesh->indices.data();

    auto ib = std::make_shared<IndexBuffer>(ib_size * sizeof(uint32_t), indices);

    if (!ib->IsValid())
    {
        Logger::Error<MeshRenderer>("Failed to create index buffer of '%d' for '%s'", ib_size,
                                    GameObject()->Path().c_str());
        buffer_creation_failed = true;
        return;
    }

    index_buffers.emplace_back(ib);

    // create index buffers for sub meshes
    for (auto i = 0; i < shared_mesh->sub_meshes.size(); i++)
    {
        const auto sub_mesh = shared_mesh->sub_meshes[i];

        const auto sub_ib_size = sub_mesh.index_count * sizeof(uint32_t);
        std::vector<uint32_t> sub_indices;
        sub_indices.insert(sub_indices.begin(),
                           shared_mesh->indices.begin() + sub_mesh.base_index,
                           shared_mesh->indices.begin() + sub_mesh.base_index + sub_mesh.index_count);

        const auto sub_ib = std::make_shared<IndexBuffer>(sub_ib_size, sub_indices.data());
        if (!sub_ib->IsValid())
        {
            Logger::Error<MeshRenderer>("Failed to create sub index buffer!: sub mesh index: %d", i);
            continue;
        }

        index_buffers.emplace_back(sub_ib);
    }
}

void MeshRenderer::UpdateBuffers()
{
    ReconstructBuffer();
    UpdateWVPBuffer();
}

void MeshRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, int material_idx)
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

            // +2 for engine pre-defined shader variables
            const int root_param_idx = shader_type * kParameterBufferType_Count + param_i + 3;
            const auto itr = material_block->Begin(shader_type, param_type);
            const auto desc_handle = itr->handle->HandleGPU;
            cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)