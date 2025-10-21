#pragma once
#include "asset_importer.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class FbxImporter : public AssetImporter
{
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
};
}