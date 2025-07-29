#include "pch.h"
#include "font_importer.h"
#include "Rendering/FontData.h"

std::vector<std::string> engine::FontImporter::SupportedExtensions()
{
    return {".spritefont"};
}

std::shared_ptr<engine::Object> engine::FontImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    auto font = Object::Instantiate<FontData>(asset->guid);
    font->LoadFont(asset->path_hint);

    return font;
}