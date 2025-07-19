#pragma once
#include "pch.h"
#include "asset_exporter.h"

class MaterialExporter : public engine::AssetExporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    void Export(engine::AssetDescriptor *descriptor) override;
};