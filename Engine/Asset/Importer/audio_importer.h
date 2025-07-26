#pragma once
#include "pch.h"
#include "asset_importer.h"

class AudioImporter : public engine::AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<engine::Object> Import(std::istream &input_stream, engine::AssetDescriptor *asset) override;
};