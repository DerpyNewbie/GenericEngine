#include "pch.h"

#include "asset_importer.h"
#include "ShaderImporter.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<AssetImporter>> AssetImporter::m_asset_importers_;

void AssetImporter::Register(const std::shared_ptr<AssetImporter> &importer)
{
    for (const auto &file_extension : importer->SupportedExtensions())
        m_asset_importers_[file_extension] = importer;
}
void AssetImporter::Register(const std::string &file_extension, const std::shared_ptr<AssetImporter> &importer)
{
    m_asset_importers_[file_extension] = importer;
}
std::shared_ptr<AssetImporter> AssetImporter::Get(const std::string &file_extension)
{
    return m_asset_importers_[file_extension];
}
}