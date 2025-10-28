#include "pch.h"
#include "assimp_util.h"

namespace AssimpUtil
{
DirectX::XMMATRIX ToXMMatrix(const aiMatrix4x4t<float> &src)
{
    return DirectX::XMMATRIX({src.a1, src.b1, src.c1, src.d1},
                             {src.a2, src.b2, src.c2, src.d2},
                             {src.a3, src.b3, src.c3, src.d3},
                             {src.a4, src.b4, src.c4, src.d4});
}

aiMatrix4x4 ToaiMatrix(const DirectX::XMMATRIX src)
{
    return aiMatrix4x4(src.r->m128_f32[0], src.r->m128_f32[4], src.r->m128_f32[8], src.r->m128_f32[12],
                       src.r->m128_f32[1], src.r->m128_f32[5], src.r->m128_f32[9], src.r->m128_f32[13],
                       src.r->m128_f32[2], src.r->m128_f32[6], src.r->m128_f32[10], src.r->m128_f32[14],
                       src.r->m128_f32[3], src.r->m128_f32[7], src.r->m128_f32[11], src.r->m128_f32[15]
        );
}

std::vector<std::string> CreateBoneNamesList(aiMesh *mesh)
{
    std::vector<std::string> bone_names{mesh->mNumBones};
    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
    {
        bone_names[i] = std::string(mesh->mBones[i]->mName.C_Str());
    }

    return bone_names;
}


Matrix GetBindPose(const std::vector<std::string> &bone_names, const aiNode *node)
{
    aiMatrix4x4 transform = node->mTransformation;
    const aiNode *parent = node->mParent;

    while (parent)
    {
        // Check if the parent node is one of the bones in our list
        bool find = false;
        for (auto bone_name : bone_names)
        {
            if (std::string(parent->mName.C_Str()) == bone_name.c_str())
            {
                find = true;
                break;
            }
        }
        if (find == false)
            return ToXMMatrix(transform);
        transform = parent->mTransformation * transform;
        parent = parent->mParent;
    }

    return ToXMMatrix(transform);
}


std::vector<Matrix> GetBindPoses(const std::vector<std::string> &bone_names, const aiMesh *mesh)
{
    std::vector<Matrix> bind_poses;
    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
        bind_poses.emplace_back(GetBindPose(bone_names, mesh->mBones[i]->mNode));
    return bind_poses;
}


aiNode *GetRootBone(const std::vector<std::string> &bone_names, aiBone *bone)
{
    aiNode *current_node = bone->mNode;
    aiNode *parent = current_node->mParent;

    while (parent)
    {
        // Check if the parent node is one of the bones in our list
        bool find = false;
        for (auto bone_name : bone_names)
        {
            if (std::string(parent->mName.C_Str()) == bone_name.c_str())
            {
                find = true;
                break;
            }
        }
        if (find == false)
            break;
        current_node = parent;
        parent = current_node->mParent;
    }
    return current_node;
}
}