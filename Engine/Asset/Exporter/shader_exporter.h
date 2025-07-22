#pragma once
#include "asset_exporter.h"

namespace engine
{
class ShaderExporter : public AssetExporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    void Export(AssetDescriptor *asset) override;
    bool CanExport(const std::shared_ptr<Object> &object) override;
};
}