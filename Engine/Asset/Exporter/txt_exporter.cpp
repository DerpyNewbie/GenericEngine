#include "pch.h"

#include "txt_exporter.h"
#include "Asset/asset_descriptor.h"
#include "Asset/text_asset.h"

namespace engine
{
class TextAsset;
std::vector<std::string> TxtExporter::SupportedExtensions()
{
    return {".txt"};
}

void TxtExporter::Export(const AssetDescriptor *asset)
{
    const auto text = std::dynamic_pointer_cast<TextAsset>(asset->managed_object);
    if (text == nullptr)
    {
        Logger::Warn<TxtExporter>("Asset '%s' has not been imported or not a TextAsset", asset->path.string().c_str());
        return;
    }
    std::ofstream file(asset->path);
    file << text->content;
    file.close();
}
}