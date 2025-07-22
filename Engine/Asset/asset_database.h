#pragma once
#include "asset_hierarchy.h"
#include "asset_ptr.h"

namespace engine
{
using namespace std::filesystem;

class AssetDatabase
{
    friend class Engine;

    static path m_project_directory_;
    static std::shared_ptr<AssetHierarchy> m_asset_hierarchy_;
    static std::unordered_map<xg::Guid, std::shared_ptr<AssetDescriptor>> m_assets_by_guid_map_;
    static std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>> m_assets_map_by_type_;

    static void Init();

public:
    static void Import(const path &path);
    static void ImportInternal(const std::shared_ptr<AssetDescriptor> &descriptor);
    static void ImportAll();

    static void SetProjectDirectory(const path &path);
    static path GetProjectDirectory();

    static std::shared_ptr<AssetHierarchy> GetRootAssetHierarchy();

    static void ReloadAsset(const xg::Guid &guid);

    static std::shared_ptr<AssetDescriptor> GetAssetDescriptor(const xg::Guid &guid);

    static IAssetPtr GetAsset(const path &path);
    static IAssetPtr GetAsset(const xg::Guid &guid);

    static std::vector<IAssetPtr> GetAssetsByType(const std::string &type);

    static void WriteAsset(AssetDescriptor *asset_descriptor);
    static void WriteAsset(const xg::Guid &guid);
    static void WriteAsset(const IAssetPtr &ptr);

    static std::shared_ptr<AssetDescriptor> CreateAsset(const std::shared_ptr<Object> &object, const path &path);

    template <typename T>
    static AssetPtr<T> GetAsset(const path &path)
    {
        return reinterpret_cast<AssetPtr<T>>(GetAsset(path));
    }

    template <typename T>
    static AssetPtr<T> GetAsset(const xg::Guid &guid)
    {
        return reinterpret_cast<AssetPtr<T>>(GetAsset(guid));
    }
};
}