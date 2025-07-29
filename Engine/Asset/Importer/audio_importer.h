#pragma once
#include "asset_importer.h"

namespace engine
{
class AudioImporter : public engine::AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<engine::Object> Import(std::istream &input_stream, engine::AssetDescriptor *asset) override;
};
}