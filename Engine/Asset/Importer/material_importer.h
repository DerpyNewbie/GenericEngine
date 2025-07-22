#pragma once
#include "asset_importer.h"

namespace engine
{
class MaterialImporter : public AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<Object> Import(AssetDescriptor *asset) override;
};
}