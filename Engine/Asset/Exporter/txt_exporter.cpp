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

void TxtExporter::Export(std::ostream &output_stream, AssetDescriptor *asset)
{
    const auto text = std::dynamic_pointer_cast<TextAsset>(asset->managed_object);
    if (text == nullptr)
    {
        Logger::Warn<TxtExporter>("Asset '%s' has not been imported or not a TextAsset",
                                  asset->path_hint.string().c_str());
        return;
    }

    output_stream << text->content;
    asset->ClearKeys();
    for (auto [key, value] : text->key_value_pairs)
    {
        asset->SetString(key, value);
    }
}

bool TxtExporter::CanExport(const std::shared_ptr<Object> &object)
{
    return std::dynamic_pointer_cast<TextAsset>(object) != nullptr;
}
}