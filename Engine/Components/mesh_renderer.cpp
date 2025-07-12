#include "pch.h"

#include "mesh_renderer.h"
#include "DxLib/dxlib_helper.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"
#include "game_object.h"
#include "camera.h"
#include "Rendering/MaterialData.h"
#include "Rendering/world_view_projection.h"

namespace engine
{
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
        if (ImGui::CollapsingHeader("Materials"))
        {
            ImGui::Indent();
            for (auto i = 0; i < shared_materials.size(); ++i)
            {
                if (ImGui::CollapsingHeader(("Material " + std::to_string(i)).c_str()))
                {
                    ImGui::Indent();
                    ImGui::PushID(i);
                    shared_materials[i]->OnInspectorGui();
                    ImGui::PopID();
                    ImGui::Unindent();
                }
            }
            ImGui::Unindent();
        }
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
}

void MeshRenderer::OnDraw()
{
    UpdateBuffers();

    auto material = shared_materials[0];
    auto shader = material->p_shared_shader.CastedLock();
    auto cmd_list = g_RenderEngine->CommandList();
    auto vbView = vertex_buffer->View();
    auto ibView = index_buffers[0]->View();
    if (material->IsValid())
    {
        if (shader)
            g_PSOManager.SetPipelineState(cmd_list, shader);

        cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd_list->IASetVertexBuffers(0, 1, &vbView);
        cmd_list->IASetIndexBuffer(&ibView);
        SetDescriptorTable(cmd_list, 0);

        cmd_list->DrawIndexedInstanced(shared_mesh->HasSubMeshes()
                                           ? shared_mesh->sub_meshes[0].base_index
                                           : shared_mesh->indices.size(), 1, 0, 0, 0);
    }
    // sub-meshes
    for (int i = 0; i < shared_mesh->sub_meshes.size(); ++i)
    {
        material = shared_materials[i + 1];
        if (material->IsValid())
        {
            shader = material->p_shared_shader.CastedLock();
            if (shader)
                g_PSOManager.SetPipelineState(cmd_list, shader);
            auto ib = index_buffers[i + 1];
            auto sub_mesh = shared_mesh->sub_meshes[i];
            ibView = ib->View();
            cmd_list->IASetIndexBuffer(&ibView);
            SetDescriptorTable(cmd_list, i + 1);
            cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
        }
    }
}

void MeshRenderer::ReconstructBuffers()
{
    if (!vertex_buffer || index_buffers.empty())
    {
        if (buffer_creation_failed)
        {
            return;
        }
        ReconstructMeshesBuffer();
    }
    material_handles.resize(shared_materials.size());
    for (int i = 0; i < shared_materials.size(); ++i)
    {
        if (shared_materials[i]->IsValid() && !shared_materials[i]->p_shared_material_block->IsCreateBuffer())
        {
            ReconstructMaterialBuffers(i);
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

void MeshRenderer::ReconstructMaterialBuffers(int material_idx)
{
    shared_materials[material_idx]->p_shared_material_block->CreateBuffer();
    //get wvp_buffer for update buffer
    auto find_material_data = shared_materials[material_idx]->p_shared_material_block->
                                                              FindMaterialDataFromName("__WVP__").lock();
    if (find_material_data)
    {
        auto wvp_data = std::dynamic_pointer_cast<MaterialData<WorldViewProjection>>(find_material_data);
        for (auto &wvp_buffer : material_wvp_buffers)
        {
            wvp_buffer.emplace_back(wvp_data);
        }
    }

    //create material handle
    ReleaseDescriptorHandles();
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            material_handles[material_idx][shader_type][params_type] = shared_materials[material_idx]->
                                                                       p_shared_material_block->GetDescriptorHandle(
                                                                           static_cast<kShaderType>(shader_type),
                                                                           static_cast<kParameterBufferType>(
                                                                               params_type));
}

void MeshRenderer::UpdateBuffers()
{
    ReconstructBuffers();
    UpdateWVPBuffer();
}

void MeshRenderer::SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, int material_idx)
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            if (material_handles[material_idx][shader_type][params_type])
                cmd_list->SetGraphicsRootDescriptorTable(
                    shader_type * 3 + params_type,
                    material_handles[material_idx][shader_type][params_type]->HandleGPU);
}

void MeshRenderer::ReleaseDescriptorHandles()
{
    for (auto material_handle : material_handles)
        for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
            for (int param_type = 0; param_type < kParameterBufferType_Count; ++param_type)
                g_DescriptorHeapManager->Get().Free(material_handle[shader_type][param_type]);
}

void MeshRenderer::UpdateWVPBuffer()
{
    if (!material_wvp_buffers[0].empty())
    {
        WorldViewProjection wvp;
        const auto camera = Camera::Main();

        wvp.WVP[0] = GameObject()->Transform()->WorldMatrix();
        wvp.WVP[1] = camera.lock()->GetViewMatrix();
        wvp.WVP[2] = camera.lock()->GetProjectionMatrix();

        for (auto &wvp_buffers : material_wvp_buffers)
        {
            for (size_t i = 0; i < wvp_buffers.size(); ++i)
            {
                if (auto wvp_buffer = wvp_buffers[i].lock())
                {
                    wvp_buffer->Set(wvp);
                    wvp_buffer->UpdateBuffer();
                }
                else
                {
                    wvp_buffers.erase(wvp_buffers.begin() + i);
                }
            }
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)