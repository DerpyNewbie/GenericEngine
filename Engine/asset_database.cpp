#include "pch.h"
#include "asset_database.h"

#include "logger.h"

#include <ranges>

namespace engine
{

path AssetDatabase::m_project_directory_;

std::shared_ptr<AssetHierarchy> AssetDatabase::m_asset_hierarchy_;

std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>>
AssetDatabase::m_assets_map_by_type_;


static std::vector<std::string> GetSplitPath(path path)
{
    auto result = std::vector<std::string>();
    while (path.has_filename())
    {
        result.insert(result.begin(), path.filename().string());
        path = path.parent_path();
    }
    return result;
}

bool AssetHierarchy::IsFile() const
{
    return asset != nullptr;
}
bool AssetHierarchy::IsDirectory() const
{
    return asset == nullptr;
}
void AssetDatabase::SetProjectDirectory(const path &path)
{
    m_project_directory_ = path;
    m_asset_hierarchy_ = Object::Instantiate<AssetHierarchy>("Assets");
    ImportAll();
}
void AssetDatabase::Import(const path &path)
{
    if (!exists(path))
    {
        Logger::Log<AssetDatabase>("Path '%s' does not exist. ignoring", path.string().c_str());
        return;
    }

    if (is_directory(path))
    {
        for (const auto &child : directory_iterator(path))
        {
            Import(child.path());
        }
        return;
    }

    if (is_regular_file(path))
    {
        auto asset_descriptor = std::make_shared<AssetDescriptor>(path);
        const auto hierarchy = GetOrCreateAssetHierarchy(path);
        hierarchy->asset = asset_descriptor;
        m_assets_map_by_type_[path.extension().string()].emplace_back(asset_descriptor);

        Logger::Log<AssetDatabase>("Asset '%s' loaded", path.string().c_str());
        return;
    }

    Logger::Warn<AssetDatabase>("Path '%s' is not a file or directory. ignoring", path.string().c_str());
}
void AssetDatabase::ImportAll()
{
    for (const auto &child : directory_iterator(m_project_directory_))
    {
        Import(child);
    }
}
path AssetDatabase::GetProjectDirectory()
{
    return m_project_directory_;
}
std::shared_ptr<AssetHierarchy> AssetDatabase::GetRootAssetHierarchy()
{
    return m_asset_hierarchy_;
}
std::shared_ptr<AssetHierarchy> AssetDatabase::GetOrCreateAssetHierarchy(const path &path)
{
    const auto rel_path = relative(path, m_project_directory_);
    const auto split_rel_path = GetSplitPath(rel_path);
    auto current_node = m_asset_hierarchy_;
    for (const auto &node : split_rel_path)
    {
        auto found_node = std::ranges::find(current_node->children, node, &AssetHierarchy::Name);
        if (found_node == current_node->children.end())
        {
            auto next = Object::Instantiate<AssetHierarchy>(node);
            next->parent = current_node;
            current_node->children.emplace_back(next);
            current_node = next;
            continue;
        }

        current_node = *found_node;
    }

    return current_node;
}
std::vector<std::shared_ptr<AssetDescriptor>> AssetDatabase::GetAssetsByType(const std::string &type)
{
    return m_assets_map_by_type_[type];
}
std::shared_ptr<AssetDescriptor> AssetDatabase::GetAssetAt(const path &path_file)
{
    const auto hierarchy = GetOrCreateAssetHierarchy(path_file);
    if (hierarchy->asset == nullptr)
    {
        Logger::Warn<AssetDatabase>("Asset '%s' not found", path_file.string().c_str());
        return nullptr;
    }
    return hierarchy->asset;
}
std::vector<std::shared_ptr<AssetDescriptor>> AssetDatabase::GetAssetsAt(const path &path_directory)
{
    const auto hierarchy = GetOrCreateAssetHierarchy(path_directory);
    auto result = std::vector<std::shared_ptr<AssetDescriptor>>();
    if (hierarchy->IsFile())
    {
        Logger::Warn<AssetDatabase>("Asset '%s' is not a directory", path_directory.string().c_str());
        result.emplace_back(hierarchy->asset);
    }

    for (const auto &child : hierarchy->children)
    {
        if (child->asset == nullptr)
            continue;
        result.emplace_back(child->asset);
    }

    return result;
}
}