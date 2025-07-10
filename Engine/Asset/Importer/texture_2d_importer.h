#pragma once
#include "asset_importer.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class Texture2DImporter : public AssetImporter
{

public:
    static IAssetPtr GetColorTexture(DirectX::PackedVector::XMCOLOR color);

    std::vector<std::string> SupportedExtensions() override;
    std::shared_ptr<Object> Import(AssetDescriptor *asset) override;
};
}