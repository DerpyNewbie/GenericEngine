#include "pch.h"

#include "AssimpLoader.h"
#include "../Graphics/SharedStruct.h"
#include "../Graphics/Texture2D.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <directx/d3dx12.h>
#include <filesystem>
#include <functional>
#include <memory>


namespace fs = std::filesystem;

aiString TruncateWtfNodePath(const aiString &nodePath)
{
    std::string case1_str("mixamorig:LeftArm_$AssimpFbx$_PreRotation");
    std::string case2_str("mixamorig:RightArm_$AssimpFbx$_PreRotation");
    std::string node_path_s = nodePath.C_Str();
    if (node_path_s.find(case1_str) != std::string::npos)
    {
        return aiString("mixamorig:LeftArm_$AssimpFbx$_Rotation");
    }
    if (node_path_s.find(case2_str) != std::string::npos)
    {
        return aiString("mixamorig:RightArm_$AssimpFbx$_Rotation");
    }

    return nodePath;
}

std::wstring GetDirectoryPath(const std::wstring &origin)
{
    fs::path p = origin.c_str();
    return p.remove_filename().c_str();
}

std::string ToUTF8(const std::wstring &value)
{
    auto length = WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);
    auto buffer = new char[length];

    WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, buffer, length, nullptr, nullptr);

    std::string result(buffer);
    delete[] buffer;
    buffer = nullptr;

    return result;
}

// std::string(マルチバイト文字列)からstd::wstring(ワイド文字列)を得る
std::wstring ToWideString(const std::string &str)
{
    auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

    std::wstring wstr;
    wstr.resize(num1);

    auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

    assert(num1 == num2);
    return wstr;
}

bool AssimpLoader::ImportModel(ModelResource settings)
{
    if (settings.FileName == nullptr)
    {
        return false;
    }

    auto &meshes = settings.Meshes;
    auto &boneOffsets = settings.Bone.Offsets;
    auto &boneRootNode = settings.Bone.RootNode;
    auto &boneTransforms = settings.Bone.Transforms;
    auto &materials = settings.Materials;
    auto inverseU = settings.inverseU;
    auto inverseV = settings.inverseV;

    auto path = ToUTF8(settings.FileName);

    Assimp::Importer importer;
    int flag = 0;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_GenSmoothNormals;
    flag |= aiProcess_GenUVCoords;
    flag |= aiProcess_RemoveRedundantMaterials;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_LimitBoneWeights;
    flag |= aiProcess_ImproveCacheLocality;
    flag |= aiProcess_OptimizeMeshes;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_EMBEDDED_TEXTURES_LEGACY_NAMING, true);

    auto scene = importer.ReadFile(path, flag);

    if (scene == nullptr)
    {
        // もし読み込みエラーがでたら表示する
        printf(importer.GetErrorString());
        printf("\n");
        return false;
    }

    // 読み込んだデータを自分で定義したMesh構造体に変換する
    meshes.clear();
    meshes.resize(scene->mNumMeshes);
    materials.resize(meshes.size());
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const auto pMesh = scene->mMeshes[i];
        LoadMeshAndBone(boneOffsets, meshes[i], pMesh, inverseU, inverseV);

        aiString texPath;
        if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            if (texPath.C_Str()[0] == '*')
            {
                auto itr = atoi(&texPath.C_Str()[1]);
                materials[i].SetTexture("default", std::make_shared<Texture2D>(scene->mTextures[itr]));
            }
        }
    }

    boneRootNode = new aiNode;
    boneRootNode->mNumChildren = scene->mRootNode->mNumChildren;
    boneRootNode->mTransformation = scene->mRootNode->mTransformation;
    boneRootNode->mChildren = new aiNode *;
    for (size_t i = 0; i < boneRootNode->mNumChildren; ++i)
    {
        boneRootNode->mChildren[i] = new aiNode;
        LoadNode(*boneRootNode->mChildren[i], *scene->mRootNode->mChildren[i], *boneRootNode);
    }
    boneTransforms.resize(boneOffsets.size());

    scene = nullptr;

    return true;
}

bool AssimpLoader::ImportAnim(AnimResource settings)
{
    if (settings.FileName == nullptr)
    {
        return false;
    }

    auto &animation = settings.Animation;

    auto path = ToUTF8(settings.FileName);

    Assimp::Importer importer;
    int flag = 0;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_LimitBoneWeights;
    flag |= aiProcess_SortByPType;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

    auto scene = importer.ReadFile(path, flag);

    if (scene == nullptr)
    {
        // もし読み込みエラーがでたら表示する
        printf(importer.GetErrorString());
        printf("\n");
        return false;
    }

    LoadAnim(animation, scene->mAnimations[0]);

    return true;
}

void AssimpLoader::LoadMeshAndBone(std::vector<aiBone> &dst_bone, CabotMesh &dst_mesh, const aiMesh *src, bool inverseU,
                                   bool inverseV)
{
    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

    dst_mesh.Position.resize(src->mNumVertices);
    dst_mesh.Normal.resize(src->mNumVertices);
    dst_mesh.Tangent.resize(src->mNumVertices);
    dst_mesh.UV.resize(src->mNumVertices);
    dst_mesh.Color.resize(src->mNumVertices);
    for (auto i = 0; i < CabotMesh::BoneNumMax; ++i)
    {
        dst_mesh.BoneIDs[i].resize(src->mNumVertices);
        dst_mesh.BoneWeights[i].resize(src->mNumVertices);
    }
    dst_mesh.BoneNum.resize(src->mNumVertices);

    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        auto position = &(src->mVertices[i]);
        auto normal = &(src->mNormals[i]);
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
        auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

        // 反転オプションがあったらUVを反転させる
        if (inverseU)
        {
            uv->x = 1 - uv->x;
        }
        if (inverseV)
        {
            uv->y = 1 - uv->y;
        }

        dst_mesh.Position[i] = DirectX::XMFLOAT3(position->x, position->y, position->z);
        dst_mesh.Normal[i] = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
        dst_mesh.UV[i] = DirectX::XMFLOAT2(uv->x, uv->y);
        dst_mesh.Tangent[i] = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
        dst_mesh.Color[i] = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);
    }

    dst_mesh.Indices.resize(src->mNumFaces * 3);

    for (auto i = 0u; i < src->mNumFaces; ++i)
    {
        const auto &face = src->mFaces[i];

        dst_mesh.Indices[i * 3 + 0] = face.mIndices[0];
        dst_mesh.Indices[i * 3 + 1] = face.mIndices[1];
        dst_mesh.Indices[i * 3 + 2] = face.mIndices[2];
    }

    dst_bone.resize(src->mNumBones);

    for (auto i = 0u; i < src->mNumBones; ++i)
    {
        auto src_bone = src->mBones[i];

        aiBone bone = {};
        bone.mName = src_bone->mName.C_Str();
        bone.mOffsetMatrix = src_bone->mOffsetMatrix;
        for (auto j = 0u; j < src_bone->mNumWeights; ++j)
        {
            auto vert_itr = src_bone->mWeights[j].mVertexId;
            auto bone_num = dst_mesh.BoneNum[vert_itr];
            dst_mesh.BoneIDs[bone_num][vert_itr] = i;
            dst_mesh.BoneWeights[bone_num][vert_itr] = src_bone->mWeights->mWeight;
            ++dst_mesh.BoneNum[vert_itr];
        }
        dst_bone[i] = bone;
    }
}

void AssimpLoader::LoadNode(aiNode &dst, aiNode &src, aiNode &parent)
{
    dst.mChildren = new aiNode *[src.mNumChildren];
    dst.mName = src.mName;
    dst.mNumChildren = src.mNumChildren;
    dst.mTransformation = src.mTransformation;
    dst.mParent = &parent;
    if (src.mNumChildren < 10)
    {
        for (size_t i = 0; i < src.mNumChildren; ++i)
        {
            dst.mChildren[i] = new aiNode;
            LoadNode(*dst.mChildren[i], *src.mChildren[i], dst);
        }
    }
    else
    {
        dst.mChildren[0] = nullptr;
    }
}

void AssimpLoader::LoadAnim(AnimationData &dst, aiAnimation *src)
{
    dst.Duration = src->mDuration;
    dst.TicksPerSecond = src->mTicksPerSecond;
    for (unsigned int i = 0; i < src->mNumChannels; ++i)
    {
        auto src_channel = src->mChannels[i];
        dst.Channels.emplace_back(new aiNodeAnim);

        auto &dst_channel = dst.Channels.back();
        dst_channel->mNodeName = src_channel->mNodeName;
        dst_channel->mNumPositionKeys = src_channel->mNumPositionKeys;
        dst_channel->mNumRotationKeys = src_channel->mNumRotationKeys;
        dst_channel->mNumScalingKeys = src_channel->mNumScalingKeys;

        dst_channel->mPositionKeys = static_cast<aiVectorKey *>(malloc(
            sizeof(aiVectorKey) * src_channel->mNumPositionKeys));
        for (unsigned int j = 0; j < src_channel->mNumPositionKeys; ++j)
        {
            auto &src_position_key = src_channel->mPositionKeys[j];
            dst_channel->mPositionKeys[j] = src_position_key;
        }
        dst_channel->mRotationKeys = static_cast<aiQuatKey *>(
            malloc(sizeof(aiQuatKey) * src_channel->mNumRotationKeys));
        for (unsigned int j = 0; j < src_channel->mNumRotationKeys; ++j)
        {
            auto &src_rotation_key = src_channel->mRotationKeys[j];
            dst_channel->mRotationKeys[j] = src_rotation_key;
        }
        dst_channel->mScalingKeys = static_cast<aiVectorKey *>(
            malloc(sizeof(aiVectorKey) * src_channel->mNumScalingKeys));
        for (unsigned int j = 0; j < src_channel->mNumScalingKeys; ++j)
        {
            auto &src_ScalingKey = src_channel->mScalingKeys[j];
            dst_channel->mScalingKeys[j] = src_ScalingKey;
        }
    }
}