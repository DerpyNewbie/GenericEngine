#pragma once
#include "asset_ptr.h"

namespace engine
{
struct AssetDescriptor : Object
{
private:
    friend class AssetDatabase;

    static constexpr auto kGuidKey = "guid";
    static constexpr auto kTypeKey = "type";
    static constexpr auto kDataKey = "data";

    rapidjson::Document m_meta_json_;

    static std::filesystem::path GetAbsoluteAssetFilePath(const std::filesystem::path &asset_path);
    static std::filesystem::path GetMetaFilePath(const std::filesystem::path &asset_path);
    static bool GetMetaJson(const std::filesystem::path &asset_path, std::string &out_json);

    static std::shared_ptr<AssetDescriptor> Read(const std::filesystem::path &path);
    static void Reload(const std::shared_ptr<AssetDescriptor> &instance, const std::string &json);

    void Write(const std::filesystem::path &path);
    rapidjson::GenericValue<rapidjson::UTF8<>> &GetDataValue();

public:
    xg::Guid guid = xg::Guid();
    std::filesystem::path path_hint = "";
    std::string type_hint;
    std::shared_ptr<Object> managed_object = nullptr; // can be null

    std::vector<std::string> GetKeys();
    void ClearKeys();

    void SetString(const std::string &key, const std::string &value);
    void SetInt(const std::string &key, int value);
    void SetFloat(const std::string &key, float value);
    void SetBool(const std::string &key, bool value);

    std::string GetString(const std::string &key);
    int GetInt(const std::string &key);
    float GetFloat(const std::string &key);
    bool GetBool(const std::string &key);

    IAssetPtr ToAssetPtr();
    void Save();
    void Reload();
};
}