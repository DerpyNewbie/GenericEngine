#pragma once
#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

namespace engine
{
DirectX::XMMATRIX aiMatrixToXMMatrix(const aiMatrix4x4t<float> &src);
aiMatrix4x4 XMMatrixToaiMatrix(DirectX::XMMATRIX src);
std::vector<std::string> CreateBoneNamesList(aiMesh *mesh);
aiMatrix4x4 GetBindPose(const std::vector<std::string> &bone_names, const aiNode *node);
std::vector<Matrix> GetBindPoses(const std::vector<std::string> &bone_names, const aiMesh *mesh);
aiNode *GetRootBone(const std::vector<std::string> &bone_names, aiBone *bone);
}