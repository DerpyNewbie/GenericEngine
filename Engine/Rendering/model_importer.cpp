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
#include "game_object.h"
#include "str_util.h"
#include "mesh.h"

// TODO: remove DxLib dependency
#include "assimp_util.h"
#include "DxLib/dxlib_converter.h"

namespace engine
{
namespace
{


std::string RetrieveNameFromPath(const char *file_path)
{
    const std::filesystem::path path = file_path;
    return path.stem().string();
}

std::shared_ptr<GameObject> CreateFromNode(const aiScene *scene, const aiNode *node,
                                           const std::shared_ptr<GameObject> &parent_node,
                                           std::map<UINT64, std::shared_ptr<GameObject>> &node_map)
{
    // create GameObject
    const auto node_go = GameObject::Instantiate<GameObject>(node->mName.C_Str());

    //set node_map
    node_map.emplace(reinterpret_cast<UINT64>(node), node_go);

    // apply transforms
    const auto node_transform = node_go->Transform();
    if (parent_node != nullptr)
    {
        node_transform->SetParent(parent_node->Transform());
    }

    node_transform->SetLocalMatrix(aiMatrixToXMMatrix(node->mTransformation));

    // create children
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        CreateFromNode(scene, node->mChildren[i], node_go, node_map);

    return node_go;
}

void AttachMeshObject(const aiScene *scene, const aiNode *node,
                      const std::shared_ptr<GameObject> &parent_node,
                      std::map<UINT64, std::shared_ptr<GameObject>> &node_map)
{
    auto node_go = node_map[reinterpret_cast<UINT64>(node)];
    // apply meshes
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::weak_ptr<Transform>> bone_transforms;
    std::unordered_set<const aiNode *> visited;
    std::weak_ptr<Transform> root_bone;

    meshes.reserve(node->mNumMeshes);
    materials.reserve(node->mNumMeshes);
    {
        unsigned int bone_count = 0;
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            bone_count += scene->mMeshes[i]->mNumBones;
        }

        bone_transforms.reserve(bone_count);
    }

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        auto current_mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(Mesh::CreateFromAiMesh(current_mesh));
        materials.emplace_back(Object::Instantiate<Material>());

        //create bone info
        auto bone_names = CreateBoneNamesList(current_mesh);
        meshes[i]->bind_poses = GetBindPoses(bone_names, current_mesh);

        for (unsigned int j = 0; j < current_mesh->mNumBones; ++j)
        {
            const aiNode *bone_node = current_mesh->mBones[j]->mNode;
            if (visited.insert(bone_node).second)
            {
                std::weak_ptr t = node_map[reinterpret_cast<std::uintptr_t>(bone_node)]->GetComponent<Transform>();
                bone_transforms.emplace_back(t);
            }
        }
        if (current_mesh->mNumBones)
        {
            //適当なボーンからルートボーンをたどっていく
            auto root_node = GetRootBone(bone_names, current_mesh->mBones[0]);
            root_bone = node_map[reinterpret_cast<std::uintptr_t>(root_node)]->Transform();
        }
    }

    if (!meshes.empty())
    {
        const auto result_mesh = meshes[0];
        for (unsigned int i = 1; i < meshes.size(); ++i)
        {
            result_mesh->Append(*meshes[i].get());
        }

        if (result_mesh->HasBoneWeights())
        {
            auto skinned_mesh_renderer = node_go->AddComponent<SkinnedMeshRenderer>();
            skinned_mesh_renderer->shared_mesh = result_mesh;
            skinned_mesh_renderer->shared_materials = materials;
            skinned_mesh_renderer->transforms = bone_transforms;
            skinned_mesh_renderer->root_bone = AssetPtr<Transform>::FromManaged(root_bone);
        }
        else
        {
            auto mesh_renderer = node_go->AddComponent<MeshRenderer>();
            mesh_renderer->shared_mesh = result_mesh;
            mesh_renderer->shared_materials = materials;
        }
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        AttachMeshObject(scene, node->mChildren[i], node_go, node_map);
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
        aiProcess_PopulateArmatureData |
        aiProcess_JoinIdenticalVertices;

    const auto scene = importer.ReadFile(file_path, import_settings);
    if (!scene)
    {
        Logger::Error<ModelImporter>("Failed to load model from FBX!");
        return nullptr;
    }
    std::map<UINT64, std::shared_ptr<GameObject>> node_map;
    auto parent_node_obj = CreateFromNode(scene, scene->mRootNode, nullptr, node_map);
    AttachMeshObject(scene, scene->mRootNode, nullptr, node_map);
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