#include "pch.h"

#include "skinned_mesh_renderer.h"

#include "Rendering/CabotEngine/Graphics/DescriptorHeapManager.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

namespace engine
{
void SkinnedMeshRenderer::OnInspectorGui()
{
    MeshRenderer::OnInspectorGui();
    
    ImGui::Checkbox("Draw Bones", &m_draw_bones_);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Bone Info"))
    {
        for (int i = 0; i < transforms.size(); i++)
        {
            //ImGui::Text("%d: %s", i, transforms[i].lock()->Name().c_str());
        }
    }

    if (ImGui::CollapsingHeader("Mesh Info"))
    {
        ImGui::Text("Vertices: %d", shared_mesh->vertices.size());
        ImGui::Text("Indices: %d", shared_mesh->indices.size());
        ImGui::Text("UVs: %d", shared_mesh->uvs[0].size());
        ImGui::Text("UV2s: %d", shared_mesh->uvs[1].size());
        ImGui::Text("Colors: %d", shared_mesh->colors.size());
        ImGui::Text("Normals: %d", shared_mesh->normals.size());
    }
}

void SkinnedMeshRenderer::OnDraw()
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

    cmd_list->SetGraphicsRootDescriptorTable(6, material_handle->HandleGPU);
    cmd_list->DrawIndexedInstanced(shared_mesh->HasSubMeshes()
                                       ? shared_mesh->sub_meshes[0].base_index
                                       : shared_mesh->indices.size(), 1, 0, 0, 0);

    for (int i = 0; i < shared_mesh->sub_meshes.size(); ++i)
    {
        auto ib = index_buffers[i + 1];
        auto sub_mesh = shared_mesh->sub_meshes[i];
        ibView = ib->View();
        cmd_list->IASetIndexBuffer(&ibView);
        cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
    }
    lineRenderer->Draw();
}

void SkinnedMeshRenderer::ReconstructBuffers()
{
    MeshRenderer::ReconstructBuffers();

    transforms.reserve(shared_mesh->bind_poses.size());
    for (size_t i = 0; i < shared_mesh->bind_poses.size(); i++)
        transforms.emplace_back(shared_mesh->bind_poses[i]);

    transforms_buffer.Initialize(transforms.size());
    transforms_buffer.Upload(transforms);

    //TODO:マテリアルが追加され次第そちらに切り替えるべし
    material_handle = g_DescriptorHeapManager->Get().Register(transforms_buffer);

    std::vector<Vertex> vertices(4);
    vertices[0].vertex = Vector3(0, 0, 0);
    vertices[0].color = Color(1, 0, 0, 1);
    vertices[1].vertex = Vector3(500, 500, 500);
    vertices[1].color = Color(0, 1, 0, 1);
    vertices[2].vertex = Vector3(500, 0, 0);
    vertices[2].color = Color(0, 0, 1, 1);
    vertices[3].vertex = Vector3(500, 0, 500);
    vertices[3].color = Color(1, 1, 1, 1);

    std::vector<unsigned int> indices(4);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
    lineRenderer = std::make_shared<LineRenderer>(vertices, indices);
}

void SkinnedMeshRenderer::UpdateBuffers()
{
    MeshRenderer::UpdateBuffers();
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)