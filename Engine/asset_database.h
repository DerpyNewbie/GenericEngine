#pragma once
#include "object.h"

namespace engine
{
using namespace std::filesystem;

struct AssetDescriptor
{
    path path;
    std::shared_ptr<Object> object; // can be null
};

class AssetHierarchy : public Object
{
public:
    std::shared_ptr<AssetDescriptor> asset; // can be null
    std::shared_ptr<AssetHierarchy> parent; // can be null
    std::vector<std::shared_ptr<AssetHierarchy>> children; // can be empty

    [[nodiscard]] bool IsFile() const;

    [[nodiscard]] bool IsDirectory() const;
};

class AssetDatabase
{
    static path m_project_directory_;
    static std::shared_ptr<AssetHierarchy> m_asset_hierarchy_;
    static std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>> m_assets_map_by_type_;

public:
    static void SetProjectDirectory(const path &path);
    static void Import(const path &path);
    static void ImportAll();

    static path GetProjectDirectory();
    static std::shared_ptr<AssetHierarchy> GetOrCreateAssetHierarchy(const path &path);
    static std::shared_ptr<AssetHierarchy> GetRootAssetHierarchy();
    static std::shared_ptr<AssetDescriptor> GetAssetAt(const path &path_file);
    static std::vector<std::shared_ptr<AssetDescriptor>> GetAssetsAt(const path &path_directory);
    static std::vector<std::shared_ptr<AssetDescriptor>> GetAssetsByType(const std::string &type);
};
}