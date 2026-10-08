#pragma once
#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

namespace AssimpUtil
{
/// <summary>
/// AssimpのMatrix(aiMatrix4x4)をDirectXのXMMATRIXに変換します。
/// </summary>
/// <param name="src">変換元のMatrix</param>
DirectX::XMMATRIX ToXMMatrix(const aiMatrix4x4t<float> &src);
/// <summary>
/// DirectXのXMMATRIXをAssimpのMatrix(aiMatrix4x4)に変換します。
/// </summary>
/// <param name="src">変換元のMatrix</param>
aiMatrix4x4 ToaiMatrix(DirectX::XMMATRIX src);
/// <summary>
/// Meshが持つすべてのBoneの名前のリストを作成します。
/// </summary>
/// <param name="mesh">対象のMesh</param>
std::vector<std::string> CreateBoneNamesList(aiMesh *mesh);
/// <summary>
/// 指定されたNodeのBindPoseを計算します。親をたどり、Boneのリストに含まれる親のTransformationのみを掛け合わせます。
/// </summary>
/// <param name="bone_names">Boneの名前のリスト</param>
/// <param name="node">対象のNode</param>
Matrix GetBindPose(const std::vector<std::string> &bone_names, const aiNode *node);
/// <summary>
/// MeshのすべてのBoneのBindPoseを計算します。
/// </summary>
/// <param name="bone_names">Boneの名前のリスト</param>
/// <param name="mesh">対象のMesh</param>
std::vector<Matrix> GetBindPoses(const std::vector<std::string> &bone_names, const aiMesh *mesh);
/// <summary>
/// 指定されたBoneから親をたどり、Boneのリストに含まれる最も上のNode(RootBone)を取得します。
/// </summary>
/// <param name="bone_names">Boneの名前のリスト</param>
/// <param name="bone">探索を始めるBone</param>
aiNode *GetRootBone(const std::vector<std::string> &bone_names, aiBone *bone);
}