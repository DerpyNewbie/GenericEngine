#include "pch.h"
#include "fbx_importer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Animation/animation_clip.h"
#include "Asset/asset_database.h"
#include "Asset/dummy_asset.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Asset/fbx_meta.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"

namespace engine
{
std::vector<std::string> FbxImporter::SupportedExtensions()
{
    return {".fbx"};
}

bool FbxImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return false;
}

void FbxImporter::OnImport(AssetDescriptor *ctx)
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
        aiProcess_LimitBoneWeights;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_EMBEDDED_TEXTURES_LEGACY_NAMING, true);

    const auto scene = importer.ReadFile(ctx->AssetPath().string().c_str(), import_settings);
    if (!scene)
    {
        Logger::Error<FbxImporter>("Failed to load model from FBX!");
        return;
    }

    // placeholder for future prefab-like implementation 
    const auto meta = Object::Instantiate<FbxMeta>(ctx->AssetPath().stem().string());
    ctx->SetMainObject(meta);

    // process fbx meta
    {
        ConversionMap convert;
        MeshNodes mesh_nodes;

        meta->root_object_meta = CreateNodeMappings(ctx, meta, scene, scene->mRootNode, convert, mesh_nodes);
        CreateTextureMappings(ctx, scene, convert);
        CreateMaterialMappings(ctx, scene, convert);

        for (const auto &mesh_node : mesh_nodes)
        {
            meta->mesh_objects.emplace(convert.to_object.at(mesh_node), CreateMesh(ctx, scene, mesh_node, convert));
        }
    }

    // process animations
    for (UINT i = 0; i < scene->mNumAnimations; i++)
    {
        CreateAnimationClip(ctx, scene->mAnimations[i]);
    }
}

std::shared_ptr<ObjectMeta> FbxImporter::CreateNodeMappings(
    AssetDescriptor *ctx,
    const std::shared_ptr<FbxMeta> &fbx_meta,
    const aiScene *ai_scene,
    const aiNode *ai_node,
    ConversionMap &out_conversion_mapping,
    MeshNodes &out_mesh_nodes
)
{
    auto object_meta = std::make_shared<ObjectMeta>();
    object_meta->parent = {};
    object_meta->name = ai_node->mName.C_Str();

    // Recurse children
    for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
    {
        auto child = CreateNodeMappings(ctx, fbx_meta, ai_scene, ai_node->mChildren[i], out_conversion_mapping, out_mesh_nodes);
        child->parent = object_meta;
        object_meta->children.push_back(child);
    }

    // Extract Transforms
    aiVector3D scaling, position;
    aiQuaternion rotation;
    ai_node->mTransformation.Decompose(scaling, rotation, position);

    TRS local_trs;
    local_trs.translation = Vector3(position.x, position.y, position.z);
    local_trs.scale = Vector3(scaling.x, scaling.y, scaling.z);
    local_trs.rotation = Quaternion(rotation.x, rotation.y, rotation.z, rotation.w);

    object_meta->local_transform = local_trs;

    if (ai_node->mNumMeshes > 0)
    {
        out_mesh_nodes.emplace(ai_node);
    }

    out_conversion_mapping.EmplaceObject(ai_node, object_meta);
    return object_meta;
}

void FbxImporter::CreateTextureMappings(AssetDescriptor *ctx, const aiScene *ai_scene, ConversionMap &out_conversion_mapping)
{
    for (UINT i = 0; i < ai_scene->mNumTextures; i++)
    {
        const auto ai_texture = ai_scene->mTextures[i];
        const auto texture = Object::Instantiate<Texture2D>();
        texture->LoadFromAiTexture(ai_texture);
        texture->SetName(ai_texture->mFilename.C_Str());
        ctx->AddObject(texture);
        out_conversion_mapping.EmplaceTexture(i, texture);
    }
}

void FbxImporter::CreateMaterialMappings(AssetDescriptor *ctx, const aiScene *ai_scene, ConversionMap &out_conversion_mapping)
{
    for (UINT i = 0; i < ai_scene->mNumMaterials; i++)
    {
        const auto ai_material = ai_scene->mMaterials[i];
        auto material_asset = Object::Instantiate<Material>();
        material_asset->SetName(ai_material->GetName().C_Str());

        aiString ai_texture_path;

        if (ai_material->GetTexture(aiTextureType_BASE_COLOR, 0, &ai_texture_path) != AI_SUCCESS)
        {
            ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_texture_path);
        }

        if (ai_texture_path.length == 0)
        {
            ctx->LogImportWarning(std::format("Material {} has no texture assigned!", ai_material->GetName().C_Str()).c_str());
        }
        else if (ai_texture_path.C_Str()[0] == '*')
        {
            // embedded texture
            const int index = std::stoi(ai_texture_path.C_Str() + 1);
            const auto texture = out_conversion_mapping.to_texture.at(index);
            const auto texture_ptr = AssetPtr<Texture2D>::FromManaged(texture);
            const auto texture_buff_data = material_asset->p_shared_material_block->GetTextureBufferData("_MainTex");
            texture_buff_data->SetTexture(texture_ptr);
        }
        else
        {
            // external texture
            const std::string file_path = ai_texture_path.C_Str();
            const auto texture = AssetDatabase::GetAsset<Texture2D>(file_path);
            const auto texture_buff_data = material_asset->p_shared_material_block->GetTextureBufferData("_MainTex");
            texture_buff_data->SetTexture(texture);
        }

        ctx->AddObject(material_asset);
        out_conversion_mapping.EmplaceMaterial(i, material_asset);
    }
}

std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>> FbxImporter::CreateMesh(
    AssetDescriptor *ctx,
    const aiScene *ai_scene,
    const aiNode *ai_node,
    const ConversionMap &convert
)
{
    const auto object_meta = convert.to_object.at(ai_node);
    if (ai_node->mNumMeshes == 0)
    {
        return {};
    }

    // populate mesh & material references
    std::vector<AssetPtr<Material>> materials;
    for (UINT i = 0; i < ai_node->mNumMeshes; i++)
    {
        const auto ai_mesh_idx = ai_node->mMeshes[i];
        const auto ai_mesh = ai_scene->mMeshes[ai_mesh_idx];
        auto converted_mesh = Mesh::CreateFromAiMesh(ai_mesh);
        if (const auto mesh = object_meta->mesh.instance.lock())
        {
            mesh->Append(*converted_mesh);

            // clean up used mesh object
            Object::Destroy(converted_mesh);
        }
        else
        {
            ctx->AddObject(converted_mesh);
            object_meta->mesh.instance = converted_mesh;
        }

const bool has_converted_material = convert.to_material.contains(ai_mesh->mMaterialIndex);
        const auto material = has_converted_material ? convert.to_material.at(ai_mesh->mMaterialIndex) : Object::Instantiate<Material>(std::format("FBX_IMPORTER_PLACEHOLDER_MATERIAL_{}_FOR_{}", ai_mesh->mMaterialIndex, converted_mesh->Name()).c_str());
        const auto material_asset = has_converted_material ? AssetPtr<Material>::FromManaged(material) : AssetPtr<Material>::FromInstance(material);

        object_meta->mesh.materials.emplace_back(material);
        materials.emplace_back(material_asset);
    }

    // populate bone references
    const auto ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[0]];
    if (ai_mesh->HasBones())
    {
        for (unsigned int j = 0; j < ai_mesh->mNumBones; ++j)
        {
            object_meta->mesh.bones.emplace_back(convert.to_object.at(ai_mesh->mBones[j]->mNode));
        }

        object_meta->mesh.root_bone = convert.to_object.at(ai_mesh->mBones[0]->mNode);
    }

    return std::make_pair(AssetPtr<Mesh>::FromManaged(object_meta->mesh.instance.lock()), materials);
}

std::shared_ptr<AnimationClip> FbxImporter::CreateAnimationClip(AssetDescriptor *ctx, const aiAnimation *ai_animation)
{
    auto anim_clip = Object::Instantiate<AnimationClip>(ai_animation->mName.C_Str());

    anim_clip->m_length_ = ai_animation->mDuration / ai_animation->mTicksPerSecond;
    anim_clip->m_frame_rate_ = static_cast<float>(1.0 / ai_animation->mTicksPerSecond);

    for (UINT j = 0; j < ai_animation->mNumChannels; ++j)
    {
        TransformAnimationCurve curve;
        auto channel = ai_animation->mChannels[j];
        std::string path = channel->mNodeName.C_Str();

        for (UINT k = 0; k < channel->mNumPositionKeys; ++k)
        {
            const auto ai_pos = channel->mPositionKeys[k].mValue;
            auto pos = Vector3(ai_pos.x, ai_pos.y, ai_pos.z);
            auto time = channel->mPositionKeys[k].mTime / ai_animation->mTicksPerSecond;
            curve.position_key.emplace_back(time, pos);
        }

        for (UINT k = 0; k < channel->mNumScalingKeys; ++k)
        {
            const auto ai_scale = channel->mScalingKeys[k].mValue;
            auto scale = Vector3(ai_scale.x, ai_scale.y, ai_scale.z);
            auto time = channel->mScalingKeys[k].mTime / ai_animation->mTicksPerSecond;
            curve.scale_key.emplace_back(time, scale);
        }

        for (UINT k = 0; k < channel->mNumRotationKeys; ++k)
        {
            auto &ai_quaternion1 = channel->mRotationKeys[k].mValue;
            ai_quaternion1.Normalize();
            if (k > 0)
            {
                const auto ai_quaternion2 = channel->mRotationKeys[k - 1].mValue;
                if (ai_quaternion1.x * ai_quaternion2.x + ai_quaternion1.y * ai_quaternion2.y + ai_quaternion1.z *
                    ai_quaternion2.z + ai_quaternion1.w * ai_quaternion2.w <= 0.0f)
                {
                    ai_quaternion1 = aiQuaternion(
                        -ai_quaternion1.w,
                        -ai_quaternion1.x,
                        -ai_quaternion1.y,
                        -ai_quaternion1.z
                    );
                }
            }
            ai_quaternion1.Normalize();
            auto rotation = Quaternion(ai_quaternion1.x, ai_quaternion1.y, ai_quaternion1.z, ai_quaternion1.w);
            auto time = channel->mPositionKeys[k].mTime / ai_animation->mTicksPerSecond;
            curve.rotation_key.emplace_back(time, rotation);
        }
        anim_clip->m_curves_.emplace(path, curve);
    }

    ctx->AddObject(anim_clip);
    return anim_clip;
}
}