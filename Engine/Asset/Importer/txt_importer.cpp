#include "pch.h"

#include "txt_importer.h"

#include "Asset/text_asset.h"
#include "logger.h"

namespace engine
{
std::vector<std::string> TxtImporter::SupportedExtensions()
{
    return {".txt"};
}
std::shared_ptr<Object> TxtImporter::Import(AssetDescriptor *asset)
{
    auto text = Object::Instantiate<TextAsset>(asset->guid);
    std::stringstream ss;
    ss << std::ifstream(asset->path_hint).rdbuf();
    text->content = ss.str();

    for (const auto &key : asset->GetKeys())
    {
        text->key_value_pairs[key] = asset->GetString(key);
    }

    asset->managed_object = text;
    return text;
}
}