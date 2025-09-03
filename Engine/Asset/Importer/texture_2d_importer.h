#pragma once
#include "asset_importer.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class Texture2DImporter : public AssetImporter
{
    enum class kImageFormat : uint8_t
    {
        kUnknown = 0,
        kWic,
        kTga
    };

    static kImageFormat GetImageFormat(const std::filesystem::path &file_path);

public:
    static IAssetPtr GetColorTexture(DirectX::PackedVector::XMCOLOR color);

    std::vector<std::string> SupportedExtensions() override;
    void OnImport(AssetDescriptor *ctx) override;
};
}