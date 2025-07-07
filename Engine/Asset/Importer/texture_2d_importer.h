#pragma once
#include "asset_importer.h"

namespace engine
{
class Texture2DImporter: public AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<Object> Import(AssetDescriptor *asset) override;
};
}