#pragma once
#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

namespace AssimpUtil
{
DirectX::XMMATRIX ToXMMatrix(const aiMatrix4x4t<float> &src);
aiMatrix4x4 ToaiMatrix(DirectX::XMMATRIX src);
std::vector<std::string> CreateBoneNamesList(aiMesh *mesh);
Matrix GetBindPose(const std::vector<std::string> &bone_names, const aiNode *node);
std::vector<Matrix> GetBindPoses(const std::vector<std::string> &bone_names, const aiMesh *mesh);
aiNode *GetRootBone(const std::vector<std::string> &bone_names, aiBone *bone);
}