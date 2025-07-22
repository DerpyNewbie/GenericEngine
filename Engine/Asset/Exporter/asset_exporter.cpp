#include "pch.h"

#include "asset_exporter.h"

namespace engine
{
std::set<std::shared_ptr<AssetExporter>> AssetExporter::m_exporters_;
std::unordered_map<std::string, std::shared_ptr<AssetExporter>> AssetExporter::m_exporter_extension_map_;

void AssetExporter::Register(const std::shared_ptr<AssetExporter> &exporter)
{
    m_exporters_.insert(exporter);
    for (const auto &file_extension : exporter->SupportedExtensions())
    {
        m_exporter_extension_map_[file_extension] = exporter;
    }
}

void AssetExporter::Register(const std::string &file_extension, const std::shared_ptr<AssetExporter> &exporter)
{
    m_exporters_.insert(exporter);
    m_exporter_extension_map_[file_extension] = exporter;
}

std::shared_ptr<AssetExporter> AssetExporter::Get(const std::string &file_extension)
{
    return m_exporter_extension_map_[file_extension];
}

std::shared_ptr<AssetExporter> AssetExporter::Get(const std::shared_ptr<Object> &object)
{
    for (auto exporter : Get())
    {
        if (exporter->CanExport(object))
        {
            return exporter;
        }
    }

    return nullptr;
}

std::set<std::shared_ptr<AssetExporter>> AssetExporter::Get()
{
    return m_exporters_;
}
}