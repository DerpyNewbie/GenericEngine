#pragma once
#include "asset_ptr.h"
#include "persistent_data_store.h"
#include "Importer/import_log.h"

namespace engine
{
/// <summary>
/// 1つのAssetファイルと、そのmetaファイルの情報を管理する構造体です。
/// </summary>
struct AssetDescriptor : enable_shared_from_base<AssetDescriptor>
{
private:
    friend class AssetDatabase;

    /// <summary>
    /// Assetに含まれるSub Objectのmeta情報です。
    /// </summary>
    struct SubObjectMeta
    {
        xg::Guid guid;
        std::string name;
        std::string type_hint;
    };

    static constexpr auto kMetaFileExtension = ".meta";
    static constexpr auto kInternalAssetPath = "__internally_generated__";
    static constexpr auto kGuidKey = "guid";
    static constexpr auto kTypeKey = "type";
    static constexpr auto kDataKey = "data";
    static constexpr auto kSubGuidsKey = "objects";
    static constexpr auto kObjectsTypeKey = "type";

    xg::Guid m_guid_ = xg::Guid();
    std::list<xg::Guid> m_sub_guids_;
    std::list<SubObjectMeta> m_sub_object_metas_;
    std::string m_type_;
    std::filesystem::path m_asset_path_;
    std::shared_ptr<Object> m_main_object_;
    std::list<std::shared_ptr<Object>> m_objects_;
    std::list<ImportLog> m_import_logs_;
    rapidjson::Document m_meta_json_;
    PersistentDataStore m_meta_data_store_;
    PersistentDataStore m_user_data_store_;

    /// <summary>
    /// Pathを絶対Pathに変換し、末尾の".meta"や区切り文字を取り除いたAssetファイルのPathを取得します。
    /// </summary>
    static std::filesystem::path AssetFilePath(const std::filesystem::path &asset_path);
    /// <summary>
    /// Assetに対応する".meta"ファイルのPathを取得します。
    /// </summary>
    static std::filesystem::path MetaFilePath(const std::filesystem::path &asset_path);
    /// <summary>
    /// Assetの".meta"ファイルの内容を読み込みます。
    /// </summary>
    /// <param name="asset_path">AssetのPath</param>
    /// <param name="out_json">読み込まれたJSON</param>
    /// <returns>".meta"ファイルが存在しない場合 false</returns>
    static bool GetMetaJson(const std::filesystem::path &asset_path, std::string &out_json);
    /// <summary>
    /// Guid、種類、ユーザーデータ、SubObjectの情報を".meta"ファイルに書き出します。
    /// </summary>
    /// <param name="path">".meta"ファイルのPath</param>
    void WriteMeta(const std::filesystem::path &path);

public:
    /// <summary>
    /// Assetの".meta"ファイルを読み込みます。存在しない場合は新しいGuidで生成します。
    /// </summary>
    /// <param name="file_path">AssetのPath</param>
    explicit AssetDescriptor(const std::filesystem::path &file_path);

    /// <summary>
    /// AssetのGuid(MainObjectのGuid)を取得します。
    /// </summary>
    [[nodiscard]] xg::Guid Guid() const;
    /// <summary>
    /// SubObjectのGuidのリストを取得します。
    /// </summary>
    [[nodiscard]] std::list<xg::Guid> SubGuids() const;
    /// <summary>
    /// AssetファイルのPathを取得します。
    /// </summary>
    [[nodiscard]] std::filesystem::path AssetPath() const;
    /// <summary>
    /// AssetのMainObjectを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<Object> MainObject() const;
    /// <summary>
    /// Assetに含まれるすべてのObject(MainObjectとSubObject)を取得します。
    /// </summary>
    [[nodiscard]] std::list<std::shared_ptr<Object>> Objects() const;
    /// <summary>
    /// ".meta"ファイルに保存されるユーザーデータのDataStoreを取得します。
    /// </summary>
    [[nodiscard]] PersistentDataStore &DataStore();
    /// <summary>
    /// Import中に記録されたログを取得します。
    /// </summary>
    [[nodiscard]] std::list<ImportLog> ImportLogs() const;
    /// <summary>
    /// Import中にエラーが記録されたかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool HasImportError() const;

    /// <summary>
    /// MainObjectを設定します。ObjectのGuidはAssetのGuidに、名前はファイル名に変更されます。
    /// </summary>
    /// <param name="object">MainObjectにするObject</param>
    void SetMainObject(std::shared_ptr<Object> object);
    /// <summary>
    /// SubObjectを追加します。MainObjectがまだない場合はMainObjectとして設定します。".meta"に同じ名前と型の記録がある場合は、そのGuidを引き継ぎます。
    /// </summary>
    /// <param name="object">追加するObject</param>
    void AddObject(std::shared_ptr<Object> object);

    /// <summary>
    /// Importのエラーを記録します。
    /// </summary>
    /// <param name="message">エラーの内容</param>
    void LogImportError(const std::string &message);
    /// <summary>
    /// Importの警告を記録します。
    /// </summary>
    /// <param name="message">警告の内容</param>
    void LogImportWarning(const std::string &message);

    /// <summary>
    /// Assetの種類に対応するAssetImporterでImportし、エラーがなければSaveします。
    /// </summary>
    void Import();
    /// <summary>
    /// AssetImporterのOnExportを呼び出し、".meta"ファイルを書き出します。内部で生成されたAssetは保存されません。
    /// </summary>
    void Save();

    /// <summary>
    /// ファイルを持たない、内部で生成されたAssetであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsInternalAsset() const;
};
}