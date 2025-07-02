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
    auto text = Object::Instantiate<TextAsset>(asset->path.string());
    std::stringstream ss;
    ss << std::ifstream(asset->path).rdbuf();
    text->content = ss.str();
    asset->managed_object = text;
    return text;
}
}