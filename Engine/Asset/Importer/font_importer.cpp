#include "font_importer.h"

#include "Rendering/FontData.h"

std::vector<std::string> FontImporter::SupportedExtensions()
{
    return {".spritefont"};
}

std::shared_ptr<engine::Object> FontImporter::Import(engine::AssetDescriptor *asset)
{
    auto font = engine::Object::Instantiate<engine::FontData>(asset->guid);
    font->LoadFont(asset->path_hint);

    return font;
}