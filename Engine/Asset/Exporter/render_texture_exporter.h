#pragma once
#include "pch.h"
#include "asset_exporter.h"

class RenderTextureExporter : public engine::AssetExporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    bool CanExport(const std::shared_ptr<engine::Object> &object) override;
    void Export(std::ostream &output_stream, engine::AssetDescriptor *descriptor) override;
};