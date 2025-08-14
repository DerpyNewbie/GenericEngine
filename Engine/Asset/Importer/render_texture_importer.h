#pragma once
#include "pch.h"
#include "asset_importer.h"

namespace engine
{
class RenderTextureImporter : public AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<Object> Import(std::istream &input_stream, AssetDescriptor *asset) override;
};
}