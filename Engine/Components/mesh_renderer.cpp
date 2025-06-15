#include "mesh_renderer.h"

#include "dxlib_helper.h"
#include "game_object.h"
#include "imgui.h"
#include "logger.h"
#include "Rendering/texture2d.h"

namespace engine
{
void MeshRenderer::OnInspectorGui()
{
    if (ImGui::CollapsingHeader("Mesh Info"))
    {
        ImGui::Text("Vertices: %llu", shared_mesh->vertices.size());
        ImGui::Text("Indices: %llu", shared_mesh->indices.size());
        ImGui::Text("Faces(calc): %llu", !shared_mesh->indices.empty() ? shared_mesh->indices.size() / 3 : 0);
        ImGui::Text("UVs: %llu", shared_mesh->uv.size());
        ImGui::Text("UV2s: %llu", shared_mesh->uv2.size());
        ImGui::Text("Colors: %llu", shared_mesh->colors.size());
        ImGui::Text("Normals: %llu", shared_mesh->normals.size());
        ImGui::Text("Tangents: %llu", shared_mesh->tangents.size());
        ImGui::Text("Bone Weights: %llu", shared_mesh->bone_weights.size());
        ImGui::Text("Bind Poses: %llu", shared_mesh->bind_poses.size());
        ImGui::Text("Sub Meshes: %llu", shared_mesh->sub_meshes.size());
    }
}

void MeshRenderer::OnDraw()
{
    if (vertex_buffer_handle == -1 || index_buffer_handles.empty())
    {
        ReconstructBuffers();
    }

    const MATRIX wtl = GameObject()->Transform()->WorldToLocal();

    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(true);
    SetUseBackCulling(true);
    MATRIX origin;
    GetTransformToWorldMatrix(&origin);
    SetTransformToWorld(&wtl);

    for (int i = 0; i < index_buffer_handles.size(); i++)
    {
        const auto ib_handle = index_buffer_handles[i];
        const auto tex_handle = (i < texture_handles.size()) ? texture_handles[i] : DX_NONE_GRAPH;
        DrawPolygonIndexed3D_UseVertexBuffer(vertex_buffer_handle, ib_handle, tex_handle, false);
    }

    SetTransformToWorld(&origin);
}

void MeshRenderer::ReconstructBuffers()
{
    // clean up old buffers
    if (vertex_buffer_handle != -1)
    {
        DeleteVertexBuffer(vertex_buffer_handle);
        vertex_buffer_handle = -1;
    }

    if (!index_buffer_handles.empty())
    {
        for (const auto ib_handle : index_buffer_handles)
            DeleteIndexBuffer(ib_handle);
        index_buffer_handles.clear();
    }

    // create vertex buffer
    vertex_buffer_handle = CreateVertexBuffer(static_cast<int>(shared_mesh->vertices.size()), DX_VERTEX_TYPE_NORMAL_3D);
    if (vertex_buffer_handle == -1)
    {
        Logger::Error<MeshRenderer>("Failed to create vertex buffer!: %s", GameObject()->Name().c_str());
        return;
    }

    // convert to dxlib-compatible type
    std::vector<VERTEX3D> raw_vertices;
    raw_vertices.reserve(shared_mesh->vertices.size());
    raw_vertices.resize(shared_mesh->vertices.size());
    for (int i = 0; i < shared_mesh->vertices.size(); i++)
    {
        raw_vertices[i].pos = shared_mesh->vertices[i];
        raw_vertices[i].norm = shared_mesh->normals[i] * -1;
        raw_vertices[i].dif = {
            .b = shared_mesh->colors[i].b,
            .g = shared_mesh->colors[i].g,
            .r = shared_mesh->colors[i].r,
            .a = shared_mesh->colors[i].a
        };
        raw_vertices[i].spc = {
            .b = shared_mesh->colors[i].b,
            .g = shared_mesh->colors[i].g,
            .r = shared_mesh->colors[i].r,
            .a = shared_mesh->colors[i].a
        };
        raw_vertices[i].u = shared_mesh->uv[i].x;
        raw_vertices[i].v = shared_mesh->uv[i].y;
        raw_vertices[i].su = shared_mesh->uv2[i].x;
        raw_vertices[i].sv = shared_mesh->uv2[i].y;
    }

    if (SetVertexBufferData(0, raw_vertices.data(), static_cast<int>(shared_mesh->vertices.size()),
                            vertex_buffer_handle) == -1)
    {
        Logger::Error<MeshRenderer>("Failed to set vertex buffer data!");
        return;
    }

    // create index buffer
    const auto ib_size = shared_mesh->sub_meshes.empty()
                             ? shared_mesh->indices.size()
                             : shared_mesh->sub_meshes[0].base_index;

    const auto ib_handle = CreateIndexBuffer(ib_size,DX_INDEX_TYPE_16BIT);

    if (ib_handle == -1)
    {
        Logger::Error<MeshRenderer>("Failed to create index buffer!");
        return;
    }

    if (SetIndexBufferData(0, shared_mesh->indices.data(), ib_size, ib_handle))
    {
        Logger::Error<MeshRenderer>("Failed to set index buffer data!");
        return;
    }

    index_buffer_handles.emplace_back(ib_handle);

    // create index buffers for sub meshes
    for (auto i = 0; i < shared_mesh->sub_meshes.size(); i++)
    {
        const auto sub_mesh = shared_mesh->sub_meshes[i];
        const auto sub_ib_handle = CreateIndexBuffer(sub_mesh.index_count, DX_INDEX_TYPE_16BIT);
        if (sub_ib_handle == -1)
        {
            Logger::Error<MeshRenderer>("Failed to create sub index buffer!: sub mesh index: %d", i);
            continue;
        }

        std::vector<int> sub_indices;
        sub_indices.insert(sub_indices.begin(),
                           shared_mesh->indices.begin() + sub_mesh.base_index,
                           shared_mesh->indices.begin() + sub_mesh.base_index + sub_mesh.index_count);

        if (SetIndexBufferData(0, sub_indices.data(), sub_indices.size(), sub_ib_handle))
        {
            Logger::Error<MeshRenderer>("Failed to set sub index buffer data for %d!", i);
            DeleteIndexBuffer(sub_ib_handle);
            continue;
        }

        index_buffer_handles.emplace_back(sub_ib_handle);
    }
}
}

CEREAL_REGISTER_TYPE(engine::MeshRenderer)