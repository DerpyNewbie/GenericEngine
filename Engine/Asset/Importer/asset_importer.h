#pragma once
#include "Asset/asset_descriptor.h"
#include "object.h"

namespace engine
{
class AssetImporter
{
    static std::unordered_map<std::string, std::shared_ptr<AssetImporter>> m_asset_importers_;

public:
    virtual ~AssetImporter() = default;
    virtual std::vector<std::string> SupportedExtensions() = 0;
    virtual std::shared_ptr<Object> Import(AssetDescriptor *asset) = 0;
    virtual void Export(AssetDescriptor *asset) = 0;

    static void AddImporter(const std::shared_ptr<AssetImporter> &importer);
    static void AddImporter(const std::string &type, const std::shared_ptr<AssetImporter> &importer);
    static std::shared_ptr<AssetImporter> GetAssetImporter(const std::string &type);
};
}