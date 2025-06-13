#include "mesh.h"
#include "Math/vector2.h"

namespace engine
{
Mesh Mesh::CreateFromMV1(int model_handle)
{
    Mesh result;

    MV1SetupReferenceMesh(model_handle, 0, false, false, 0);
    const auto polygon_list = MV1GetReferenceMesh(model_handle, 0, false, false, 0);
    result.vertices.resize(polygon_list.VertexNum);
    result.uv.resize(polygon_list.VertexNum);
    result.uv2.resize(polygon_list.VertexNum);
    result.colors.resize(polygon_list.VertexNum);
    result.normals.resize(polygon_list.VertexNum);

    for (int i = 0; i < polygon_list.VertexNum; i++)
    {
        const auto vertex = polygon_list.Vertexs[i];
        result.vertices[i] = vertex.Position;
        result.uv[i] = vertex.TexCoord[0];
        result.uv2[i] = vertex.TexCoord[1];
        result.colors[i] = vertex.DiffuseColor;
        result.normals[i] = vertex.Normal;
    }

    result.indices.resize(static_cast<size_t>(polygon_list.PolygonNum * 3));
    for (int i = 0; i < polygon_list.PolygonNum; i++)
    {
        const auto polygon = polygon_list.Polygons[i];
        result.indices[i * 3] = polygon.VIndex[0];
        result.indices[i * 3 + 1] = polygon.VIndex[1];
        result.indices[i * 3 + 2] = polygon.VIndex[2];
    }

    return result;
}
}