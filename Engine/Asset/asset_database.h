#pragma once
#include "asset_hierarchy.h"
#include "asset_ptr.h"

namespace engine
{
using namespace std::filesystem;

/// <summary>
/// Project内のAssetのImportと検索、Assetの階層の管理を行うクラスです。
/// </summary>
class AssetDatabase
{
    friend class Engine;

    static path m_project_directory_;
    static std::shared_ptr<AssetHierarchy> m_asset_hierarchy_;
    static std::unordered_map<xg::Guid, std::shared_ptr<AssetDescriptor>> m_assets_by_guid_map_;
    static std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>> m_assets_map_by_type_;

    /// <summary>
    /// 標準のAssetImporterをすべて登録し、実行ディレクトリの"Resources"をProjectDirectoryに設定します。
    /// </summary>
    static void Init();

public:
    /// <summary>
    /// 指定されたファイル、またはDirectory以下のすべてのファイルをImportし、AssetHierarchyに追加します。".meta"ファイルは無視されます。
    /// </summary>
    /// <param name="path">ImportするファイルまたはDirectoryのPath</param>
    static void Import(const path &path);
    /// <summary>
    /// ファイルを持たない、内部で生成されたAssetとしてDescriptorを登録します。
    /// </summary>
    /// <param name="descriptor">登録するDescriptor</param>
    static void ImportInternal(const std::shared_ptr<AssetDescriptor> &descriptor);
    /// <summary>
    /// ProjectDirectory以下のすべてのファイルをImportします。
    /// </summary>
    static void ImportAll();

    /// <summary>
    /// ProjectDirectoryを設定し、AssetHierarchyを作り直してすべてのファイルをImportします。
    /// </summary>
    /// <param name="path">ProjectDirectoryのPath</param>
    static void SetProjectDirectory(const path &path);
    /// <summary>
    /// ProjectDirectoryのPathを取得します。
    /// </summary>
    static path GetProjectDirectory();

    /// <summary>
    /// 指定されたPathがProjectDirectoryの中にあるかどうかを判定します。
    /// </summary>
    static bool IsInProjectDirectory(const path &path);

    /// <summary>
    /// ProjectDirectoryに対応する、RootのAssetHierarchyを取得します。
    /// </summary>
    static std::shared_ptr<AssetHierarchy> GetRootAssetHierarchy();
    /// <summary>
    /// 指定されたPathに対応するAssetHierarchyを取得します。
    /// </summary>
    /// <param name="path">ファイルまたはDirectoryのPath</param>
    /// <returns>見つからない場合 nullptr</returns>
    static std::shared_ptr<AssetHierarchy> GetAssetHierarchy(const path &path);

    /// <summary>
    /// 指定されたGuidのAssetをImportし直します。
    /// </summary>
    /// <param name="guid">AssetのGuid</param>
    static void Reimport(const xg::Guid &guid);

    /// <summary>
    /// GuidからAssetDescriptorを取得します。SubObjectのGuidでも取得できます。
    /// </summary>
    /// <param name="guid">AssetのGuid</param>
    /// <returns>見つからない場合 nullptr</returns>
    static std::shared_ptr<AssetDescriptor> GetAssetDescriptor(const xg::Guid &guid);

    /// <summary>
    /// 指定されたPathのAssetを取得します。まだImportされていない場合はImportします。
    /// </summary>
    /// <param name="path">AssetのPath</param>
    /// <returns>見つからない場合は何も参照していないAssetPtr</returns>
    static IAssetPtr GetAsset(const path &path);
    /// <summary>
    /// 指定されたGuidのAssetを取得します。
    /// </summary>
    /// <param name="guid">AssetのGuid</param>
    /// <returns>見つからない場合は何も参照していないAssetPtr</returns>
    static IAssetPtr GetAsset(const xg::Guid &guid);

    /// <summary>
    /// 指定された種類のAssetをすべて取得します。
    /// </summary>
    /// <param name="type">Assetの種類(".txt"などの拡張子)</param>
    static std::vector<IAssetPtr> GetAssetsByType(const std::string &type);

    /// <summary>
    /// Assetをファイルに書き出します。
    /// </summary>
    /// <param name="asset_descriptor">書き出すAssetのDescriptor</param>
    static void WriteAsset(AssetDescriptor *asset_descriptor);
    /// <summary>
    /// 指定されたGuidのAssetをファイルに書き出します。
    /// </summary>
    /// <param name="guid">AssetのGuid</param>
    static void WriteAsset(const xg::Guid &guid);
    /// <summary>
    /// AssetPtrが参照しているAssetをファイルに書き出します。
    /// </summary>
    /// <param name="ptr">書き出すAssetへのAssetPtr</param>
    static void WriteAsset(const IAssetPtr &ptr);

    /// <summary>
    /// Objectを新しいAssetとして指定されたPathに書き出し、Importします。
    /// </summary>
    /// <param name="object">AssetにするObject</param>
    /// <param name="path">書き出し先のPath。ProjectDirectoryの中である必要があります。</param>
    /// <returns>作成されたAssetのDescriptor。失敗した場合 nullptr</returns>
    static std::shared_ptr<AssetDescriptor> CreateAsset(const std::shared_ptr<Object> &object, const path &path);
    /// <summary>
    /// Assetのファイルと".meta"ファイルを削除し、AssetHierarchyを破棄します。
    /// </summary>
    /// <param name="path">削除するAssetのPath</param>
    static void DeleteAsset(const path &path);

    /// <summary>
    /// 指定されたPathのAssetを、T のAssetPtrとして取得します。
    /// </summary>
    /// <param name="path">AssetのPath</param>
    template <typename T>
    static AssetPtr<T> GetAsset(const path &path)
    {
        return AssetPtr<T>::FromIAssetPtr(GetAsset(path));
    }

    /// <summary>
    /// 指定されたGuidのAssetを、T のAssetPtrとして取得します。
    /// </summary>
    /// <param name="guid">AssetのGuid</param>
    template <typename T>
    static AssetPtr<T> GetAsset(const xg::Guid &guid)
    {
        return reinterpret_cast<AssetPtr<T>>(GetAsset(guid));
    }
};
}