#pragma once
#include "pch.cpp"
#include "asset_exporter.h"

namespace engine
{
class MaterialExporter : public AssetExporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    void Export(std::ostream &output_stream, AssetDescriptor *descriptor) override;
    bool CanExport(const std::shared_ptr<Object> &object) override;
};
}