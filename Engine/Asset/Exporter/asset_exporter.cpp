#include "pch.h"

#include "asset_exporter.h"

namespace engine
{
std::unordered_map<std::string, std::shared_ptr<AssetExporter>> AssetExporter::m_asset_exporters_;

void AssetExporter::Register(const std::shared_ptr<AssetExporter> &exporter)
{
    for (const auto &file_Extension : exporter->SupportedExtensions())
        m_asset_exporters_[file_Extension] = exporter;
}
void AssetExporter::Register(const std::string &file_extension, const std::shared_ptr<AssetExporter> &exporter)
{
    m_asset_exporters_[file_extension] = exporter;
}
std::shared_ptr<AssetExporter> AssetExporter::Get(const std::string &file_extension)
{
    return m_asset_exporters_[file_extension];
}
}