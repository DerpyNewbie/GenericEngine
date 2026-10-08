#pragma once

namespace engine
{
/// <summary>
/// FBXファイルからModelを読み込むクラスです。
/// </summary>
class ModelImporter
{
public:
    /// <summary>
    /// FBXのAssetを取得し、GameObjectとしてInstantiateします。
    /// </summary>
    /// <param name="file_path">FBXファイルのPath</param>
    /// <returns>生成されたRootのGameObject。失敗した場合 nullptr</returns>
    static std::shared_ptr<GameObject> LoadModelFromFBX(const char *file_path);
};
}