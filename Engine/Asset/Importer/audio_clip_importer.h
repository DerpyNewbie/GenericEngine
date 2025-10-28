#pragma once
#include "asset_importer.h"

namespace engine
{
class AudioClipImporter : public AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
};
}