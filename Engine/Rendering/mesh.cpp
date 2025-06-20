#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif

#include <array>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "DxLib/dxlib_converter.h"
#include "mesh.h"
#include "logger.h"

namespace engine
{
std::shared_ptr<Mesh> Mesh::CreateFromAiMesh(const aiScene *scene, const aiMesh *mesh)
{
    const auto result = Instantiate<Mesh>(mesh->mName.C_Str());
    Logger::Log<Mesh>("Creating mesh `%s` from aiMesh", result->Name().c_str());
    Logger::Log<Mesh>("  - %d vertices", mesh->mNumVertices);
    Logger::Log<Mesh>("  - %d indices", mesh->mNumFaces * 3);

    // copy vertices
    result->vertices.resize(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        result->vertices[i] = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    }

    // copy colors
    if (mesh->GetNumColorChannels() > 0 && mesh->HasVertexColors(0))
    {
        result->colors.resize(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            result->colors[i] = Color(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b,
                                      mesh->mColors[0][i].a);
        }
    }

    // copy UVs
    for (unsigned int i = 0; i < mesh->GetNumUVChannels(); ++i)
    {
        result->GetUV(i)->resize(mesh->mNumVertices);
        for (unsigned int j = 0; j < mesh->mNumUVComponents[i]; ++j)
        {
            result->GetUV(i)->at(j) = Vector2(mesh->mTextureCoords[i][j].x, mesh->mTextureCoords[i][j].y);
        }
    }

    // copy indices
    if (mesh->HasFaces())
    {
        // FIXME: maybe check mPrimitiveTypes for new capacity
        result->indices.reserve(mesh->mNumFaces * 3ULL);
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const auto &face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                result->indices.emplace_back(face.mIndices[j]);
            }
        }
    }

    // copy normals
    if (mesh->HasNormals())
    {
        result->normals.resize(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            result->normals[i] = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
    }

    // copy tangents
    if (mesh->HasTangentsAndBitangents())
    {
        result->tangents.resize(mesh->mNumVertices);

        if (mesh->HasNormals())
        {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                Vector3 tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                Vector3 normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
                Vector3 bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                const float w = bitangent.Dot(tangent.Cross(normal));
                result->tangents[i] = Vector4(tangent.x, tangent.y, tangent.z, w);
            }
        }
        else
        {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                result->tangents[i] = Vector4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 1.0F);
            }
        }
    }

    // copy bones
    if (mesh->HasBones())
    {
        // vector of vertex id { pair of bone index and vertex weight}
        std::vector<std::vector<std::pair<int, aiVertexWeight *>>> vertex_bone_map;
        vertex_bone_map.resize(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumBones; ++i)
        {
            const auto bone = mesh->mBones[i];
            for (unsigned int j = 0; j < bone->mNumWeights; ++j)
            {
                auto weight = bone->mWeights[j];
                auto &pair_map = vertex_bone_map.at(weight.mVertexId);

                pair_map.emplace_back(i, &weight);
            }
        }
        // copy bone weights
        result->bone_weights.resize(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            const auto &pair_map = vertex_bone_map.at(i);

            result->bone_weights.reserve(pair_map.size());
            for (auto [index, vert_weight] : pair_map)
            {
                result->bone_weights[i].emplace_back(index, vert_weight->mWeight);
            }
        }
    }
    return result;
}
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
        result.emplace_back(std::move(mesh));
        MV1TerminateReferenceMesh(model_handle, frame_index, false, false, mesh_index);
    }
    return result;
}
std::shared_ptr<Mesh> Mesh::CreateFromMV1ReferenceMesh(const MV1_REF_POLYGONLIST &mv1_ref_polygon_list)
{
    auto result = Instantiate<Mesh>();
    result->vertices.resize(mv1_ref_polygon_list.VertexNum);
    result->uvs[0].resize(mv1_ref_polygon_list.VertexNum);
    result->uvs[1].resize(mv1_ref_polygon_list.VertexNum);
    result->colors.resize(mv1_ref_polygon_list.VertexNum);
    result->normals.resize(mv1_ref_polygon_list.VertexNum);

    for (int i = 0; i < mv1_ref_polygon_list.VertexNum; i++)
    {
        const auto vertex = mv1_ref_polygon_list.Vertexs[i];
        result->vertices[i] = DxLibConverter::To(vertex.Position);
        result->uvs[0][i] = DxLibConverter::To(vertex.TexCoord[0]);
        result->uvs[1][i] = DxLibConverter::To(vertex.TexCoord[1]);
        result->colors[i] = DxLibConverter::To(vertex.DiffuseColor);
        result->normals[i] = DxLibConverter::To(vertex.Normal);
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

    SubMesh sub_mesh = {.base_vertex = static_cast<int>(vertices.size()),
                        .vertex_count = static_cast<int>(other.vertices.size()),
                        .base_index = static_cast<int>(indices.size()),
                        .index_count = static_cast<int>(other.indices.size())};
    sub_meshes.emplace_back(sub_mesh);

    for (auto &index : other.indices)
    {
        index += sub_mesh.base_vertex;
    }

    vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
    // TODO: may require additional checks for appending to empty uv
    for (size_t i = 0; i < uvs.size(); i++)
        uvs[0].insert(uvs[0].end(), other.uvs[0].begin(), other.uvs[0].end());
    uvs[1].insert(uvs[1].end(), other.uvs[1].begin(), other.uvs[1].end());
    uvs[2].insert(uvs[2].end(), other.uvs[2].begin(), other.uvs[2].end());
    uvs[3].insert(uvs[3].end(), other.uvs[3].begin(), other.uvs[3].end());
    uvs[4].insert(uvs[4].end(), other.uvs[4].begin(), other.uvs[4].end());
    uvs[5].insert(uvs[5].end(), other.uvs[5].begin(), other.uvs[5].end());
    uvs[6].insert(uvs[6].end(), other.uvs[6].begin(), other.uvs[6].end());
    uvs[7].insert(uvs[7].end(), other.uvs[7].begin(), other.uvs[7].end());
    colors.insert(colors.end(), other.colors.begin(), other.colors.end());
    normals.insert(normals.end(), other.normals.begin(), other.normals.end());
    indices.insert(indices.end(), other.indices.begin(), other.indices.end());
}
std::vector<Vector2> *Mesh::GetUV(const int index)
{
    return &uvs[index];
}
bool Mesh::HasUV(const int index) const
{
    return !uvs[index].empty();
}
bool Mesh::HasTangents() const
{
    return !tangents.empty();
}
bool Mesh::HasNormals() const
{
    return !normals.empty();
}
bool Mesh::HasColors() const
{
    return !colors.empty();
}
bool Mesh::HasBoneWeights() const
{
    return !bone_weights.empty();
}
bool Mesh::HasBindPoses() const
{
    return !bind_poses.empty();
}
bool Mesh::HasSubMeshes() const
{
    return !sub_meshes.empty();
}

}

CEREAL_REGISTER_TYPE(engine::Mesh)