#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "DxLib/dxlib_converter.h"
#include "mesh.h"
#include "CabotEngine/Converter/D3D12ToAssimp.h"

namespace
{
using namespace engine;
std::shared_ptr<Mesh> CreateFromMV1ReferenceMesh(const MV1_REF_POLYGONLIST &mv1_ref_polygon_list)
{
    auto result = Object::Instantiate<Mesh>();
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
}

namespace engine
{

aiMatrix4x4 GetGlobalTransform(const aiNode *node)
{
    aiMatrix4x4 transform = node->mTransformation;
    while (node->mParent)
    {
        node = node->mParent;
        transform = node->mTransformation * transform;
    }
    return transform;
}

aiMatrix4x4 GetBindPose(const aiBone *bone)
{
    if (!bone->mNode)
    {
        throw std::runtime_error("bone->mNode == nullptr: " +
                                 std::string(bone->mName.C_Str()));
    }

    aiMatrix4x4 global = GetGlobalTransform(bone->mNode);
    aiMatrix4x4 skinMat = global * bone->mOffsetMatrix;

    return skinMat;
}

std::shared_ptr<Mesh> Mesh::CreateFromAiMesh(const aiMesh *mesh)
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
        result->bone_weights.resize(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumBones; ++i)
        {
            auto bone = mesh->mBones[i];
            result->bind_poses.emplace_back(aiMatrixToXMMatrix(GetBindPose(bone)));
            for (unsigned int j = 0; j < bone->mNumWeights; ++j)
            {
                auto weight = mesh->mBones[i]->mWeights[j];
                auto vert_itr = weight.mVertexId;
                BoneWeight bone_weight;
                bone_weight.bone_index = i;
                bone_weight.weight = weight.mWeight;
                result->bone_weights[vert_itr].emplace_back(bone_weight);
                if (result->max_bones_in_vertex < result->bone_weights[vert_itr].size())
                {
                    result->max_bones_in_vertex = result->bone_weights[vert_itr].size();
                }
            }
        }
        // copy bone weights

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
    indices.insert(indices.end(), other.indices.begin(), other.indices.end());

    for (size_t i = 0; i < uvs.size(); i++)
    {
        if (HasUV(i) || other.HasUV(i))
            uvs[i].resize(sub_mesh.base_vertex);
        uvs[i].insert(uvs[i].end(), other.uvs[i].begin(), other.uvs[i].end());
    }

    // copy colors
    {
        if (HasColors() || other.HasColors())
            colors.resize(sub_mesh.base_vertex);

        colors.insert(colors.end(), other.colors.begin(), other.colors.end());
    }

    // copy normals
    {
        if (HasNormals() || other.HasNormals())
            normals.resize(sub_mesh.base_vertex);

        normals.insert(normals.end(), other.normals.begin(), other.normals.end());
    }

    // copy tangents
    {
        if (HasTangents() || other.HasTangents())
            tangents.resize(sub_mesh.base_vertex);

        tangents.insert(tangents.end(), other.tangents.begin(), other.tangents.end());
    }

    // copy bone-weights
    {
        if (HasBoneWeights() || other.HasBoneWeights())
            bone_weights.resize(sub_mesh.base_vertex);
        bone_weights.insert(bone_weights.end(), other.bone_weights.begin(), other.bone_weights.end());
    }

    assert(bind_poses.size() == other.bind_poses.size() && "Bind pose must be same");

    max_bones_in_vertex = max(max_bones_in_vertex, other.max_bones_in_vertex);
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