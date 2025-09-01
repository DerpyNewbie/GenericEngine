#include "pch.h"
#include "fbx_importer.h"

#include "Animation/animation_clip.h"
#include "Asset/dummy_asset.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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

    const auto scene = importer.ReadFile(ctx->AssetPath().string().c_str(), import_settings);
    if (!scene)
    {
        Logger::Error<FbxImporter>("Failed to load model from FBX!");
        return;
    }

    // placeholder for future prefab-like implementation 
    ctx->SetMainObject(Object::Instantiate<DummyAsset>());

    // TODO: implement importer in Mesh -> Material -> Animation order
    for (UINT i = 0; i < scene->mNumMeshes; ++i)
    {
        ctx->AddObject(Object::Instantiate<DummyAsset>());
    }

    for (UINT i = 0; i < scene->mNumMaterials; ++i)
    {
        ctx->AddObject(Object::Instantiate<DummyAsset>());
    }

    for (UINT i = 0; i < scene->mNumAnimations; ++i)
    {
        const auto animation = scene->mAnimations[i];
        TransformAnimationCurve curve;
        auto anim_clip = Object::Instantiate<AnimationClip>();
        for (UINT j = 0; j < animation->mNumChannels; ++j)
        {
            auto channel = animation->mChannels[j];
            std::string path = channel->mNodeName.C_Str();

            for (UINT k = 0; k < channel->mNumPositionKeys; ++k)
            {
                const auto ai_pos = channel->mPositionKeys[k].mValue;
                auto pos = Vector3(ai_pos.x, ai_pos.y, ai_pos.z);
                curve.position_key.emplace(channel->mPositionKeys[k].mTime, pos);
            }

            for (UINT k = 0; k < channel->mNumScalingKeys; ++k)
            {
                const auto ai_scale = channel->mScalingKeys[k].mValue;
                auto scale = Vector3(ai_scale.x, ai_scale.y, ai_scale.z);
                curve.scale_key.emplace(channel->mScalingKeys[k].mTime, scale);
            }

            for (UINT k = 0; k < channel->mNumRotationKeys; ++k)
            {
                const auto ai_rotation = channel->mRotationKeys[k].mValue;
                auto rotation = Quaternion(ai_rotation.x, ai_rotation.y, ai_rotation.z, ai_rotation.w);
                curve.rotation_key.emplace(channel->mRotationKeys[k].mTime, rotation);
            }
            anim_clip->m_curves_.emplace(path, curve);
            ctx->AddObject(anim_clip);
        }
    }
}
}