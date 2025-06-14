#include "mesh.h"

#include <utility>

#include "logger.h"
#include "Math/vector2.h"

namespace engine
{
std::vector<std::shared_ptr<Mesh>> Mesh::CreateFromMV1(const int model_handle, const int frame_index)
{
    auto mesh_num = MV1GetFrameMeshNum(model_handle, frame_index);
    if (mesh_num == 0)
    {
        Logger::Error<Mesh>("CreateFromMV1: No mesh found for handle %d", model_handle);
        return {};
    }

    std::vector<std::shared_ptr<Mesh>> result;
    for (int i = 0; i < mesh_num; i++)
    {
        const auto mesh_index = MV1GetFrameMesh(model_handle, frame_index, i);
        if (mesh_index == -1)
        {
            Logger::Error<Mesh>("CreateFromMV1: Failed to get mesh index for handle %d", model_handle);
            return {};
        }

        if (MV1SetupReferenceMesh(model_handle, frame_index, false, false, mesh_index))
        {
            Logger::Error<Mesh>("CreateFromMV1: Failed to setup reference mesh for handle %d", model_handle);
            continue;
        }

        const auto polygon_list = MV1GetReferenceMesh(model_handle, frame_index, false, false, mesh_index);
        if (polygon_list.VertexNum == 0)
        {
            Logger::Error<Mesh>("CreateFromMV1: No polygon found for handle %d", model_handle);
            MV1TerminateReferenceMesh(model_handle, frame_index, false, false, mesh_index);
            continue;
        }

        auto mesh = CreateFromMV1ReferenceMesh(polygon_list);
        auto mesh_triangle_list_num = MV1GetMeshTListNum(model_handle, mesh_index);
        for (int j = 0; j < mesh_triangle_list_num; j++)
        {
            auto mesh_triangle_list_index = MV1GetMeshTList(model_handle, mesh_index, j);

            // MV1GetTriangleListPolygonVertexPosition()
        }

        result.emplace_back(std::move(mesh));

        MV1TerminateReferenceMesh(model_handle, frame_index, false, false, mesh_index);
    }
    //
    // auto frame_num = MV1GetFrameNum(model_handle);
    // result->bind_poses.resize(frame_num);
    // for (int i = 0; i < frame_num; i++)
    // {
    //     result->bind_poses[i] = MV1GetFrameBaseLocalMatrix(model_handle, i);
    // }

    return result;
}
std::shared_ptr<Mesh> Mesh::CreateFromMV1ReferenceMesh(const MV1_REF_POLYGONLIST &mv1_ref_polygon_list)
{
    auto result = Instantiate<Mesh>();
    result->vertices.resize(mv1_ref_polygon_list.VertexNum);
    result->uv.resize(mv1_ref_polygon_list.VertexNum);
    result->uv2.resize(mv1_ref_polygon_list.VertexNum);
    result->colors.resize(mv1_ref_polygon_list.VertexNum);
    result->normals.resize(mv1_ref_polygon_list.VertexNum);

    for (int i = 0; i < mv1_ref_polygon_list.VertexNum; i++)
    {
        const auto vertex = mv1_ref_polygon_list.Vertexs[i];
        result->vertices[i] = vertex.Position;
        result->uv[i] = vertex.TexCoord[0];
        result->uv2[i] = vertex.TexCoord[1];
        result->colors[i] = vertex.DiffuseColor;
        result->normals[i] = vertex.Normal;
    }

    result->indices.resize(3ULL * mv1_ref_polygon_list.PolygonNum);
    for (size_t i = 0; std::cmp_less(i, mv1_ref_polygon_list.PolygonNum); i++)
    {
        const auto polygon = mv1_ref_polygon_list.Polygons[i];
        result->indices[i * 3] = polygon.VIndex[0];
        result->indices[i * 3 + 1] = polygon.VIndex[1];
        result->indices[i * 3 + 2] = polygon.VIndex[2];
    }

    return result;
}
void Mesh::Append(Mesh other)
{
    if (other.vertices.empty() && other.indices.empty())
        return;

    sub_meshes.emplace_back(static_cast<int>(vertices.size()), other.vertices.size(),
                            static_cast<int>(indices.size()), other.indices.size());

    // for (auto &index : other.indices)
    // {
    //     index += vertices.size() - 2;
    // }

    vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
    uv.insert(uv.end(), other.uv.begin(), other.uv.end());
    uv2.insert(uv2.end(), other.uv2.begin(), other.uv2.end());
    colors.insert(colors.end(), other.colors.begin(), other.colors.end());
    normals.insert(normals.end(), other.normals.begin(), other.normals.end());
    indices.insert(indices.end(), other.indices.begin(), other.indices.end());
}
}

CEREAL_REGISTER_TYPE(engine::Mesh)