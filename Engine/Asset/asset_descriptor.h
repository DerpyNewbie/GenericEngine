#pragma once
#include "asset_ptr.h"
#include "persistent_data_store.h"
#include "Importer/import_log.h"

namespace engine
{
struct AssetDescriptor : enable_shared_from_base<AssetDescriptor>
{
private:
    friend class AssetDatabase;

    static constexpr auto kMetaFileExtension = ".meta";
    static constexpr auto kInternalAssetPath = "__internally_generated__";
    static constexpr auto kGuidKey = "guid";
    static constexpr auto kTypeKey = "type";
    static constexpr auto kDataKey = "data";
    static constexpr auto kSubGuidsKey = "objects";
    static constexpr auto kObjectsTypeKey = "type";

    xg::Guid m_guid_ = xg::Guid();
    std::list<xg::Guid> m_sub_guids_;
    std::string m_type_;
    std::filesystem::path m_asset_path_;
    std::shared_ptr<Object> m_main_object_;
    std::list<std::shared_ptr<Object>> m_objects_;
    std::list<ImportLog> m_import_logs_;
    rapidjson::Document m_meta_json_;
    PersistentDataStore m_meta_data_store_;
    PersistentDataStore m_user_data_store_;

    static std::filesystem::path AssetFilePath(const std::filesystem::path &asset_path);
    static std::filesystem::path MetaFilePath(const std::filesystem::path &asset_path);
    static bool GetMetaJson(const std::filesystem::path &asset_path, std::string &out_json);
    void WriteMeta(const std::filesystem::path &path);

public:
    explicit AssetDescriptor(const std::filesystem::path &file_path);

    [[nodiscard]] xg::Guid Guid() const;
    [[nodiscard]] std::list<xg::Guid> SubGuids() const;
    [[nodiscard]] std::filesystem::path AssetPath() const;
    [[nodiscard]] std::shared_ptr<Object> MainObject() const;
    [[nodiscard]] std::list<std::shared_ptr<Object>> Objects() const;
    [[nodiscard]] PersistentDataStore &DataStore();
    [[nodiscard]] std::list<ImportLog> ImportLogs() const;

    void SetMainObject(std::shared_ptr<Object> object);
    void AddObject(std::shared_ptr<Object> object);

    void LogImportError(const std::string &message);
    void LogImportWarning(const std::string &message);

    void Import();
    void Save();

    [[nodiscard]] bool IsInternalAsset() const;
};
}