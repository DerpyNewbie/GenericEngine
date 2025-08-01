#include "pch.h"
#include "font_importer.h"
#include "Rendering/font_data.h"

namespace engine
{
std::vector<std::string> FontImporter::SupportedExtensions()
{
    return {".spritefont"};
}

std::shared_ptr<Object> FontImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    auto font = Object::Instantiate<FontData>(asset->guid);
    font->LoadFont(asset->path_hint);

    return font;
}
}