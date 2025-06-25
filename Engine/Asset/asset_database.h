#pragma once
#include "asset_hierarchy.h"
#include "asset_descriptor.h"
#include "object.h"

namespace engine
{
using namespace std::filesystem;

class AssetDatabase
{
    friend class Engine;

    static path m_project_directory_;
    static std::shared_ptr<AssetHierarchy> m_asset_hierarchy_;
    static std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>> m_assets_map_by_type_;

    static void Init();

public:
    static void SetProjectDirectory(const path &path);
    static void Import(const path &path);
    static void ImportAll();

    static path GetProjectDirectory();
    static std::shared_ptr<AssetHierarchy> GetOrCreateAssetHierarchy(const path &path);
    static std::shared_ptr<AssetHierarchy> GetRootAssetHierarchy();
    static std::shared_ptr<AssetDescriptor> GetAssetDescriptor(const path &path_file);
    static std::vector<std::shared_ptr<AssetDescriptor>> GetAssetDescriptors(const path &path_directory);
    static std::vector<std::shared_ptr<AssetDescriptor>> GetAssetDescriptorsByType(const std::string &type);
    static std::shared_ptr<Object> GetAsset(const path &path);
    static std::vector<std::shared_ptr<Object>> GetAssetsByType(const std::string &type);
    static void SaveAsset(const path &path);
};
}