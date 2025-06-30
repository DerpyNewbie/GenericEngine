#include "pch.h"

#include "mesh_renderer.h"

#include <array>
#include "DxLib/dxlib_helper.h"
#include "DxLib/dxlib_converter.h"
#include "Rendering/Vertex.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"
#include "imgui.h"
#include "game_object.h"
#include "logger.h"
#include "camera.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeapManager.h"

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

    auto cmd_list = g_RenderEngine->CommandList();
    auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
    auto vbView = vertex_buffer->View();
    auto ibView = index_buffers[0]->View();

    cmd_list->SetPipelineState(g_PSOManager.Get("Basic"));

    cmd_list->SetGraphicsRootConstantBufferView(0, wvp_buffers[currentIndex]->GetAddress());

    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);
    cmd_list->IASetIndexBuffer(&ibView);
    //cmd_list->DrawIndexedInstanced(shared_mesh->HasSubMeshes()
    //? shared_mesh->sub_meshes[0].base_index
    //: shared_mesh->indices.size(),1, 0, 0, 0);
    // sub-meshes
    for (int i = 0; i < shared_mesh->sub_meshes.size(); ++i)
    {
        auto ib = index_buffers[i + 1];
        auto sub_mesh = shared_mesh->sub_meshes[i];
        ibView = ib->View();
        cmd_list->IASetIndexBuffer(&ibView);
        //cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, sub_mesh.base_vertex, 0);
    }
}

void MeshRenderer::ReconstructBuffers()
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

    for (auto &wvp_buffer : wvp_buffers)
    {
        wvp_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix) * 3);
        if (!wvp_buffer->IsValid())
        {
            Logger::Error<MeshRenderer>("Failed to create WVP buffer!");
        }
    }

    material_handles.resize(shared_materials.size());
        for (auto &[key,value] : shared_materials[0]->shared_material_block->params_tex2d)
            if (key == "diffuse")
                material_handles[0] = g_DescriptorHeapManager->Get().Register(value);
}

void MeshRenderer::UpdateBuffers()
{
    if (!vertex_buffer || index_buffers.empty())
    {
        if (buffer_creation_failed)
        {
            return;
        }

        ReconstructBuffers();
    }

    const auto camera = Camera::Main();
    Matrix world = GameObject()->Transform()->WorldMatrix();
    const Matrix view = camera.lock()->GetViewMatrix();
    const Matrix proj = camera.lock()->GetProjectionMatrix();

    for (auto &wvp_buffer : wvp_buffers)
    {
        auto ptr = wvp_buffer->GetPtr<Matrix>();
        ptr[0] = world;
        ptr[1] = view;
        ptr[2] = proj;
    }
}

}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)