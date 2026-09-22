#pragma once
#include "asset_importer.h"
#include "Components/component.h"
#include "Rendering/compute_shader.h"

namespace engine
{
class DepthTextureImporter : public engine::AssetImporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;
    void OnExport(AssetDescriptor *ctx) override;
};
}