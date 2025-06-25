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
    auto text = Object::Instantiate<TextAsset>(asset->path.string().c_str());
    std::stringstream ss;
    ss << std::ifstream(asset->path).rdbuf();
    text->content = ss.str();
    return text;
}
void TxtImporter::Export(AssetDescriptor *asset)
{
    const auto text = std::dynamic_pointer_cast<TextAsset>(asset->object);
    if (text == nullptr)
    {
        Logger::Warn<TxtImporter>("Asset '%s' has not been imported or not a TextAsset", asset->path.string().c_str());
        return;
    }
    std::ofstream file(asset->path);
    file << text->content;
    file.close();
}
}