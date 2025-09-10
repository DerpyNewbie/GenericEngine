#pragma once
#include "Asset/Importer/asset_importer.h"

namespace engine
{
class TxtImporter : public AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
    void OnExport(AssetDescriptor *ctx) override;
};
}