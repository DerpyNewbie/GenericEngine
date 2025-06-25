#include "pch.h"

#include "asset_importer.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<AssetImporter>> AssetImporter::m_asset_importers_;

void AssetImporter::AddImporter(const std::shared_ptr<AssetImporter> &importer)
{
    for (const auto &type : importer->SupportedExtensions())
        m_asset_importers_[type] = importer;
}
void AssetImporter::AddImporter(const std::string &type, const std::shared_ptr<AssetImporter> &importer)
{
    m_asset_importers_[type] = importer;
}
std::shared_ptr<AssetImporter> AssetImporter::GetAssetImporter(const std::string &type)
{
    return m_asset_importers_[type];
}
}