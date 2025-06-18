#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif

#include "model_importer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Components/frame_meta_data.h"
#include "Components/mesh_renderer.h"
#include "Components/skinned_mesh_renderer.h"
#include "DxLib/dxlib_converter.h"
#include "game_object.h"
#include "str_util.h"
#include "logger.h"
#include "mesh.h"

namespace engine
{
namespace
{
Matrix ConvertAi(const aiMatrix4x4 &m)
{
    // TODO: conversion may not be accurate as rotation is invalid
    Matrix result;
    result.m[0][0] = m.a1;
    result.m[0][1] = m.a2;
    result.m[0][2] = m.a3;
    result.m[0][3] = m.a4;
    result.m[1][0] = m.b1;
    result.m[1][1] = m.b2;
    result.m[1][2] = m.b3;
    result.m[1][3] = m.b4;
    result.m[2][0] = m.c1;
    result.m[2][1] = m.c2;
    result.m[2][2] = m.c3;
    result.m[2][3] = m.c4;
    result.m[3][0] = m.d1;
    result.m[3][1] = m.d2;
    result.m[3][2] = m.d3;
    result.m[3][3] = m.d4;
    return result;
}
std::string RetrieveNameFromPath(const char *file_path)
{
    const std::filesystem::path path = file_path;
    return path.stem().string();
}
std::shared_ptr<GameObject> CreateFromNode(const aiScene *scene, const aiNode *node,
                                           const std::shared_ptr<GameObject> &parent_node)
{
    // create GameObject
    const auto node_go = GameObject::Instantiate<GameObject>(node->mName.C_Str());

    // apply transforms
    const auto node_transform = node_go->Transform();
    if (parent_node != nullptr)
    {
        node_transform->SetParent(parent_node->Transform());
    }

    // TODO: local matrix is incorrect
    node_transform->SetLocalMatrix(ConvertAi(node->mTransformation));

    // apply meshes
    std::vector<std::shared_ptr<Mesh>> meshes;
    meshes.reserve(node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        meshes.emplace_back(Mesh::CreateFromAiMesh(scene, scene->mMeshes[node->mMeshes[i]]));
    }

    if (!meshes.empty())
    {
        const auto result_mesh = meshes[0];
        for (unsigned int i = 1; i < meshes.size(); ++i)
        {
            result_mesh->Append(*meshes[i].get());
        }
        
        node_go->AddComponent<MeshRenderer>()->shared_mesh = result_mesh;
    }

    // create children
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        CreateFromNode(scene, node->mChildren[i], node_go);

    return node_go;
}
}

std::shared_ptr<GameObject> ModelImporter::LoadModelFromFBX(const char *file_path)
{
    Assimp::Importer importer;

    constexpr int import_settings =
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_SortByPType |
        aiProcess_OptimizeMeshes |
        aiProcess_PopulateArmatureData;

    const auto scene = importer.ReadFile(file_path, import_settings);
    if (!scene)
    {
        Logger::Error<ModelImporter>("Failed to load model from FBX!");
        return nullptr;
    }

    auto parent_node_obj = CreateFromNode(scene, scene->mRootNode, nullptr);
    parent_node_obj->SetName(RetrieveNameFromPath(file_path));
    return parent_node_obj;
}

std::shared_ptr<GameObject> ModelImporter::LoadModelFromMV1(const char *file_path)
{
    const auto model_handle = MV1LoadModel(file_path);
    if (model_handle == -1)
    {
        Logger::Error<ModelImporter>("Failed to load model `%s` from MV1!", file_path);
        return nullptr;
    }

    const auto root = Object::Instantiate<GameObject>(RetrieveNameFromPath(file_path));
    const auto root_transform = root->Transform();

    const auto frame_count = MV1GetFrameNum(model_handle);
    auto frame_transforms = std::vector<std::weak_ptr<Transform>>(frame_count);
    auto frame_local_matrix = std::vector<Matrix>(frame_count);

    // Populate frame transforms
    for (int i = 0; i < frame_count; ++i)
    {
        const auto frame_name = StringUtil::ShiftJisToUtf8(std::string(MV1GetFrameName(model_handle, i)));
        const auto frame_obj = Object::Instantiate<GameObject>(frame_name);
        frame_transforms[i] = frame_obj->Transform();
        frame_local_matrix[i] = DxLibConverter::To(MV1GetFrameBaseLocalMatrix(model_handle, i));

        const auto frame_data = frame_obj->AddComponent<FrameMetaData>();
        frame_data->max_vert_pos = DxLibConverter::To(MV1GetFrameMaxVertexLocalPosition(model_handle, i));
        frame_data->min_vert_pos = DxLibConverter::To(MV1GetFrameMinVertexLocalPosition(model_handle, i));
        frame_data->avg_vert_pos = DxLibConverter::To(MV1GetFrameAvgVertexLocalPosition(model_handle, i));
        frame_data->bind_pose = DxLibConverter::To(MV1GetFrameBaseLocalMatrix(model_handle, i));
        frame_data->verts = MV1GetFrameVertexNum(model_handle, i);
        frame_data->tris = MV1GetFrameTriangleNum(model_handle, i);
        frame_data->meshes = MV1GetFrameMeshNum(model_handle, i);

        if (MV1GetFrameMeshNum(model_handle, i) > 0)
        {
            for (const auto meshes = Mesh::CreateFromMV1(model_handle, i); auto &mesh : meshes)
            {
                frame_obj->AddComponent<MeshRenderer>()->shared_mesh = mesh;
            }
        }
    }

    // Create parent-child relations
    for (int i = 0; i < frame_count; ++i)
    {
        const auto frame_parent = MV1GetFrameParent(model_handle, i);
        const auto frame_transform = frame_transforms[i].lock();
        frame_transform->SetParent(frame_parent <= -1 ? root_transform : frame_transforms[frame_parent]);
        frame_transform->SetLocalMatrix(frame_local_matrix[i]);
    }

    return root;
}
}