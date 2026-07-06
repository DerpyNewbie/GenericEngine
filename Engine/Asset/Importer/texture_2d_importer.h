#pragma once
#include "asset_importer.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class Texture2DImporter : public AssetImporter
{

public:
    static AssetPtr<Texture2D> GetColorTexture(DirectX::PackedVector::XMCOLOR color);

    std::vector<std::string> SupportedExtensions() override;
    void OnImport(AssetDescriptor *ctx) override;
};
}