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
        auto anim_clip = Object::Instantiate<AnimationClip>();

        anim_clip->m_length_ = animation->mDuration / animation->mTicksPerSecond;
        anim_clip->m_frame_rate_ = 1.0 / animation->mTicksPerSecond;

        for (UINT j = 0; j < animation->mNumChannels; ++j)
        {
            TransformAnimationCurve curve;
            auto channel = animation->mChannels[j];
            std::string path = channel->mNodeName.C_Str();

            for (UINT k = 0; k < channel->mNumPositionKeys; ++k)
            {
                const auto ai_pos = channel->mPositionKeys[k].mValue;
                auto pos = Vector3(ai_pos.x, ai_pos.y, ai_pos.z);
                auto time = channel->mPositionKeys[k].mTime / animation->mTicksPerSecond;
                curve.position_key.emplace_back(time, pos);
            }

            for (UINT k = 0; k < channel->mNumScalingKeys; ++k)
            {
                const auto ai_scale = channel->mScalingKeys[k].mValue;
                auto scale = Vector3(ai_scale.x, ai_scale.y, ai_scale.z);
                auto time = channel->mPositionKeys[k].mTime / animation->mTicksPerSecond;
                curve.scale_key.emplace_back(time, scale);
            }

            for (UINT k = 0; k < channel->mNumRotationKeys; ++k)
            {
                auto &ai_quaternion1 = channel->mRotationKeys[k].mValue;
                if (k != 0)
                {
                    auto ai_quaternion2 = channel->mRotationKeys[k - 1].mValue;
                    if (ai_quaternion1.x * ai_quaternion2.x + ai_quaternion1.y * ai_quaternion2.y + ai_quaternion1.z *
                        ai_quaternion2.z + ai_quaternion1.w * ai_quaternion2.w <= 0.0f)
                    {
                        ai_quaternion1 = aiQuaternion(-ai_quaternion1.w, -ai_quaternion1.x, -ai_quaternion1.y,
                                                      -ai_quaternion1.z);
                    }
                }
                ai_quaternion1.Normalize();
                auto rotation = Quaternion(ai_quaternion1.x, ai_quaternion1.y, ai_quaternion1.z, ai_quaternion1.w);
                auto time = channel->mPositionKeys[k].mTime / animation->mTicksPerSecond;
                curve.rotation_key.emplace_back(time, rotation);
            }
            anim_clip->m_curves_.emplace(path, curve);
        }
        ctx->AddObject(anim_clip);
    }
}
}