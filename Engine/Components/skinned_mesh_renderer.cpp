#include "skinned_mesh_renderer.h"

#include "dxlib_helper.h"
#include "imgui.h"
#include "Rendering/texture2d.h"

namespace engine
{
void SkinnedMeshRenderer::OnInspectorGui()
{
    ImGui::Checkbox("Draw Bones", &m_draw_bones_);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Bone Info"))
    {
        for (int i = 0; i < transforms.size(); i++)
        {
            ImGui::Text("%d: %s", i, transforms[i].lock()->Name().c_str());
        }
    }

    if (ImGui::CollapsingHeader("Mesh Info"))
    {
        ImGui::Text("Vertices: %d", shared_mesh->vertices.size());
        ImGui::Text("Indices: %d", shared_mesh->indices.size());
        ImGui::Text("UVs: %d", shared_mesh->uv.size());
        ImGui::Text("UV2s: %d", shared_mesh->uv2.size());
        ImGui::Text("Colors: %d", shared_mesh->colors.size());
        ImGui::Text("Normals: %d", shared_mesh->normals.size());
    }

    ImGui::Text("Texture: %d", m_texture_.graphic_handle);
}

void SkinnedMeshRenderer::OnDraw()
{
    if (vertex_buffer_handle == -1 || index_buffer_handle == -1)
    {
        ReconstructBuffers();
    }

    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(true);
    DrawPolygonIndexed3D_UseVertexBuffer(
        vertex_buffer_handle, index_buffer_handle,
        m_texture_.graphic_handle, false);
}

void SkinnedMeshRenderer::ReconstructBuffers()
{
    if (vertex_buffer_handle != -1)
    {
        DeleteVertexBuffer(vertex_buffer_handle);
        vertex_buffer_handle = -1;
    }

    if (index_buffer_handle != -1)
    {
        DeleteIndexBuffer(index_buffer_handle);
        index_buffer_handle = -1;
    }

    if (shared_mesh->vertices.empty())
    {
        Logger::Warn<SkinnedMeshRenderer>("Mesh has no vertices!");
        return;
    }

    vertex_buffer_handle = CreateVertexBuffer(static_cast<int>(shared_mesh->vertices.size()), DX_VERTEX_TYPE_NORMAL_3D);
    if (vertex_buffer_handle == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("Failed to create vertex buffer!");
        return;
    }

    std::vector<VERTEX3D> raw_vertices;
    raw_vertices.reserve(shared_mesh->vertices.size());
    raw_vertices.resize(shared_mesh->vertices.size());
    for (int i = 0; i < shared_mesh->vertices.size(); i++)
    {
        raw_vertices[i].pos = shared_mesh->vertices[i];
        raw_vertices[i].norm = shared_mesh->normals[i];
        raw_vertices[i].dif = {shared_mesh->colors[i].b, shared_mesh->colors[i].g, shared_mesh->colors[i].r,
                               shared_mesh->colors[i].a};
        raw_vertices[i].spc = {shared_mesh->colors[i].b, shared_mesh->colors[i].g, shared_mesh->colors[i].r,
                               shared_mesh->colors[i].a};
        raw_vertices[i].u = shared_mesh->uv[i].x;
        raw_vertices[i].v = shared_mesh->uv[i].y;
        raw_vertices[i].su = shared_mesh->uv2[i].x;
        raw_vertices[i].sv = shared_mesh->uv2[i].y;
    }

    if (SetVertexBufferData(0, raw_vertices.data(), static_cast<int>(shared_mesh->vertices.size()),
                            vertex_buffer_handle) == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("Failed to set vertex buffer data!");
        return;
    }

    index_buffer_handle = CreateIndexBuffer(static_cast<int>(shared_mesh->indices.size()), DX_INDEX_TYPE_16BIT);
    if (index_buffer_handle == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("Failed to create index buffer!");
        return;
    }

    if (SetIndexBufferData(0, shared_mesh->indices.data(), static_cast<int>(shared_mesh->indices.size()),
                           index_buffer_handle) == -1)
    {
        Logger::Error<SkinnedMeshRenderer>("Failed to set index buffer data!");
        return;
    }

    m_texture_ = Texture2D::GetColoredTexture({0xFF, 0xFF, 0xFF, 0xFF});
}
}

CEREAL_REGISTER_TYPE(engine::SkinnedMeshRenderer)