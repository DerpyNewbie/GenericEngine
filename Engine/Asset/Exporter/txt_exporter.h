#pragma once
#include "asset_exporter.h"

namespace engine
{
class TxtExporter : public AssetExporter
{
public:
    std::vector<std::string> SupportedExtensions() override;
    void Export(const AssetDescriptor *asset) override;
};
}