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
#include "Components/mesh_renderer.h"
#include "Components/skinned_mesh_renderer.h"
#include "game_object.h"
#include "mesh.h"
#include "assimp_util.h"

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

    node_transform->SetLocalMatrix(AssimpUtil::ToXMMatrix(node->mTransformation));

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
    std::vector<AssetPtr<Material>> materials;
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
        materials.emplace_back(AssetPtr<Material>::FromManaged(Object::Instantiate<Material>()));

        // create bone info
        auto bone_names = AssimpUtil::CreateBoneNamesList(current_mesh);
        meshes[i]->bind_poses = AssimpUtil::GetBindPoses(bone_names, current_mesh);

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
            // trace root bone from a random bone
            auto root_node = AssimpUtil::GetRootBone(bone_names, current_mesh->mBones[0]);
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
            skinned_mesh_renderer->SetSharedMesh(result_mesh);
            skinned_mesh_renderer->shared_materials = materials;
            skinned_mesh_renderer->transforms = bone_transforms;
            skinned_mesh_renderer->root_bone = AssetPtr<Transform>::FromManaged(root_bone);
            skinned_mesh_renderer->inverted_bind_poses.resize(result_mesh->bind_poses.size());
            for (int i = 0; i < result_mesh->bind_poses.size(); ++i)
            {
                skinned_mesh_renderer->inverted_bind_poses[i] = result_mesh->bind_poses[i].Invert();
            }
        }
        else
        {
            auto mesh_renderer = node_go->AddComponent<MeshRenderer>();
            mesh_renderer->SetSharedMesh(result_mesh);
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
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

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
}