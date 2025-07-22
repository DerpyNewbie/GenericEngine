#include "pch.h"

#include "asset_importer.h"
#include "shader_importer.h"

namespace engine
{
std::set<std::shared_ptr<AssetImporter>> AssetImporter::m_importers_;
std::unordered_map<std::string, std::shared_ptr<AssetImporter>> AssetImporter::m_importer_extension_map_;

void AssetImporter::Register(const std::shared_ptr<AssetImporter> &importer)
{
    for (const auto &file_extension : importer->SupportedExtensions())
        m_importer_extension_map_[file_extension] = importer;
}

void AssetImporter::Register(const std::string &file_extension, const std::shared_ptr<AssetImporter> &importer)
{
    m_importer_extension_map_[file_extension] = importer;
}

std::shared_ptr<AssetImporter> AssetImporter::Get(const std::string &file_extension)
{
    return m_importer_extension_map_[file_extension];
}

std::set<std::shared_ptr<AssetImporter>> AssetImporter::Get()
{
    return m_importers_;
}
}