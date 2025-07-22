#include "pch.h"
#include "asset_database.h"

#include "Asset/Importer/asset_importer.h"
#include "Asset/Importer/txt_importer.h"
#include "logger.h"
#include "Exporter/asset_exporter.h"
#include "Exporter/material_exporter.h"
#include "Exporter/shader_exporter.h"
#include "Exporter/txt_exporter.h"
#include "Importer/shader_importer.h"
#include "Importer/material_importer.h"
#include "Importer/texture_2d_importer.h"

namespace engine
{
path AssetDatabase::m_project_directory_;
std::shared_ptr<AssetHierarchy> AssetDatabase::m_asset_hierarchy_;
std::unordered_map<xg::Guid, std::shared_ptr<AssetDescriptor>> AssetDatabase::m_assets_by_guid_map_;
std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>> AssetDatabase::m_assets_map_by_type_;

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

void AssetDatabase::Init()
{
    AssetImporter::Register(std::make_shared<TxtImporter>());
    AssetExporter::Register(std::make_shared<TxtExporter>());
    AssetImporter::Register(std::make_shared<ShaderImporter>());
    AssetExporter::Register(std::make_shared<ShaderExporter>());
    AssetImporter::Register(std::make_shared<MaterialImporter>());
    AssetExporter::Register(std::make_shared<MaterialExporter>());
    AssetImporter::Register(std::make_shared<Texture2DImporter>());
    SetProjectDirectory(current_path() / "Resources");
}

void AssetDatabase::Import(const path &path)
{
    if (!exists(path))
    {
        Logger::Log<AssetDatabase>("Path '%s' does not exist. ignoring", path.string().c_str());
        return;
    }

    if (path.string().ends_with(".meta"))
    {
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
        const auto rel_path = relative(path, m_project_directory_);
        auto split_rel_path = GetSplitPath(rel_path);
        auto current_node = m_asset_hierarchy_;
        int depth = 0;
        std::string current_node_path;
        for (const auto &node : split_rel_path)
        {
            ++depth;
            if (!current_node_path.empty())
                current_node_path += "/";
            current_node_path += node;

            auto found_node = std::ranges::find(current_node->children, node, &AssetHierarchy::Name);
            if (found_node == current_node->children.end())
            {
                auto next = Object::Instantiate<AssetHierarchy>(node);
                if (depth < split_rel_path.size())
                    next->m_is_directory_ = true;
                next->asset = AssetDescriptor::Read(current_node_path);
                next->parent = current_node;
                current_node->children.emplace_back(next);
                current_node = next;

                m_assets_by_guid_map_[next->asset->guid] = next->asset;
                m_assets_map_by_type_[next->asset->type_hint].emplace_back(next->asset);
                continue;
            }

            current_node = *found_node;
        }

        current_node->m_is_file_ = true;

        Logger::Log<AssetDatabase>("Asset '%s' loaded", path.string().c_str());
        return;
    }

    Logger::Warn<AssetDatabase>("Path '%s' is not a file or directory. ignoring", path.string().c_str());
}

void AssetDatabase::ImportInternal(const std::shared_ptr<AssetDescriptor> &descriptor)
{
    descriptor->path_hint = AssetDescriptor::kInternalAssetPath;
    m_assets_by_guid_map_[descriptor->guid] = descriptor;
}

void AssetDatabase::ImportAll()
{
    Import(m_project_directory_);
}

void AssetDatabase::SetProjectDirectory(const path &path)
{
    m_project_directory_ = path;
    m_asset_hierarchy_ = Object::Instantiate<AssetHierarchy>("Root");
    m_asset_hierarchy_->m_is_directory_ = true;
    ImportAll();
}

path AssetDatabase::GetProjectDirectory()
{
    return m_project_directory_;
}

bool AssetDatabase::IsInProjectDirectory(const path &path)
{
    const auto path_a = weakly_canonical(path).string();
    const auto path_b = weakly_canonical(m_project_directory_).string();
    return path_a.starts_with(path_b);
}

std::shared_ptr<AssetHierarchy> AssetDatabase::GetRootAssetHierarchy()
{
    return m_asset_hierarchy_;
}

std::shared_ptr<AssetHierarchy> AssetDatabase::GetAssetHierarchy(const path &path)
{
    const auto relative_path = relative(weakly_canonical(path), m_project_directory_);
    const auto split_relative_path = GetSplitPath(relative_path);
    auto current_node = m_asset_hierarchy_;
    for (const auto &node : split_relative_path)
    {
        auto found_node = std::ranges::find(current_node->children, node, &AssetHierarchy::Name);
        if (found_node == current_node->children.end())
        {
            Logger::Error<AssetDatabase>("Asset '%s' not found", path.string().c_str());
            return nullptr;
        }

        current_node = *found_node;
    }

    return current_node;
}

void AssetDatabase::ReloadAsset(const xg::Guid &guid)
{
    const auto asset_desc_it = m_assets_by_guid_map_.find(guid);
    if (asset_desc_it == m_assets_by_guid_map_.end())
    {
        Logger::Error<AssetDatabase>("Asset with guid '%s' not found", guid.str().c_str());
        return;
    }

    asset_desc_it->second->Reload();
}

std::shared_ptr<AssetDescriptor> AssetDatabase::GetAssetDescriptor(const xg::Guid &guid)
{
    if (!m_assets_by_guid_map_.contains(guid))
        return nullptr;
    return m_assets_by_guid_map_[guid];
}

IAssetPtr AssetDatabase::GetAsset(const path &path)
{
    const auto peek = AssetDescriptor::Read(path);
    const auto guid = peek->guid;
    auto asset_desc_it = m_assets_by_guid_map_.find(guid);
    if (asset_desc_it == m_assets_by_guid_map_.end())
    {
        Logger::Log<AssetDatabase>("Asset '%s' not found. Importing...", path.string().c_str());
        Import(path);
        asset_desc_it = m_assets_by_guid_map_.find(guid);
        if (asset_desc_it == m_assets_by_guid_map_.end())
        {
            Logger::Error<AssetDatabase>("Asset '%s' not found", path.string().c_str());
            return {};
        }
    }

    const auto asset_desc = asset_desc_it->second;
    return IAssetPtr::FromAssetDescriptor(asset_desc);
}

IAssetPtr AssetDatabase::GetAsset(const xg::Guid &guid)
{
    const auto asset_desc_it = m_assets_by_guid_map_.find(guid);
    if (asset_desc_it == m_assets_by_guid_map_.end())
    {
        Logger::Error<AssetDatabase>("Asset with guid '%s' not found", guid.str().c_str());
        return {};
    }

    const auto asset_desc = asset_desc_it->second;
    return IAssetPtr::FromAssetDescriptor(asset_desc);
}

std::vector<IAssetPtr> AssetDatabase::GetAssetsByType(const std::string &type)
{
    auto assets = std::vector<IAssetPtr>();
    const auto assets_it = m_assets_map_by_type_.find(type);
    if (assets_it == m_assets_map_by_type_.end())
    {
        Logger::Warn<AssetDatabase>("No assets of type '%s' found", type.c_str());
        return assets;
    }

    const auto asset_descriptors = assets_it->second;
    for (const auto &asset_descriptor : asset_descriptors)
    {
        assets.emplace_back(IAssetPtr::FromAssetDescriptor(asset_descriptor));
    }

    return assets;
}

void AssetDatabase::WriteAsset(AssetDescriptor *asset_descriptor)
{
    if (asset_descriptor->IsInternalAsset())
    {
        Logger::Warn<AssetDatabase>("Cannot write internal asset '%s'", asset_descriptor->guid.str().c_str());
        return;
    }

    const auto exporter = AssetExporter::Get(asset_descriptor->type_hint);
    if (exporter == nullptr)
    {
        Logger::Warn<AssetDatabase>("Asset exporter for type '%s' not found! Will ignore file '%s'",
                                    asset_descriptor->type_hint.c_str(), asset_descriptor->path_hint.c_str());
        return;
    }

    exporter->Export(asset_descriptor);
    asset_descriptor->Save();
}

void AssetDatabase::WriteAsset(const xg::Guid &guid)
{
    const auto asset_desc_it = m_assets_by_guid_map_.find(guid);
    if (asset_desc_it == m_assets_by_guid_map_.end())
    {
        Logger::Error<AssetDatabase>("Could not write asset as asset with guid '%s' is not found.", guid.str().c_str());
        return;
    }

    const auto asset_descriptor = asset_desc_it->second;
    WriteAsset(asset_descriptor.get());
}

void AssetDatabase::WriteAsset(const IAssetPtr &ptr)
{
    WriteAsset(ptr.Guid());
}

std::shared_ptr<AssetDescriptor> AssetDatabase::CreateAsset(const std::shared_ptr<Object> &object, const path &path)
{
    if (path.empty() || exists(path))
    {
        Logger::Error("Path '%s' is empty or already exists. Cannot create asset!", path.string().c_str());
        return nullptr;
    }

    if (!IsInProjectDirectory(path))
    {
        Logger::Error("Path '%s' is outside of project path. Cannot create asset!", path.string().c_str());
        assert(false && "Invalid path specified for CreateAsset");
    }

    auto descriptor = GetAssetDescriptor(object->Guid());
    if (descriptor != nullptr)
    {
        if (!descriptor->IsInternalAsset())
        {
            Logger::Error("Asset with guid %s already exists. Cannot create asset!", object->Guid().str().c_str());
            return nullptr;
        }

        descriptor->managed_object = object;
        descriptor->path_hint = path;
    }
    else
    {
        descriptor = std::make_shared<AssetDescriptor>();
        descriptor->guid = object->Guid();
        descriptor->path_hint = path;
        descriptor->managed_object = object;
        descriptor->type_hint = AssetExporter::Get(object)->SupportedExtensions().front();
    }

    WriteAsset(descriptor.get());
    Import(path);
    return descriptor;
}

void AssetDatabase::DeleteAsset(const path &path)
{
    auto absolute_path = absolute(path);
    if (!exists(absolute_path))
    {
        Logger::Error("Path '%s' does not exist. Cannot delete asset!", absolute_path.string().c_str());
        return;
    }

    if (!IsInProjectDirectory(absolute_path))
    {
        Logger::Error("Path '%s' is outside of project path. Cannot delete asset!", absolute_path.string().c_str());
        assert(false && "Invalid path specified for DeleteAsset");
    }

    std::filesystem::remove(absolute_path);
    std::filesystem::remove(AssetDescriptor::GetMetaFilePath(absolute_path));
    Object::Destroy(GetAssetHierarchy(absolute_path));
}
}