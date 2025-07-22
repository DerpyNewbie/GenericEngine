#pragma once
#include "Asset/Importer/asset_importer.h"

namespace engine
{
class TxtImporter : public AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<Object> Import(const std::istream &input_stream, AssetDescriptor *asset) override;
};
}