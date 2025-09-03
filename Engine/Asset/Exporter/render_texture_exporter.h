#pragma once
#include "asset_exporter.h"

namespace engine
{
class RenderTextureExporter : public AssetExporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    bool CanExport(const std::shared_ptr<Object> &object) override;
    void Export(std::ostream &output_stream, AssetDescriptor *descriptor) override;
};
}