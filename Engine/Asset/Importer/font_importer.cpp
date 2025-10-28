#include "pch.h"
#include "font_importer.h"
#include "Rendering/font_data.h"

namespace engine
{
std::vector<std::string> FontImporter::SupportedExtensions()
{
    return {".spritefont"};
}

bool FontImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<FontData>(object) != nullptr;
}

void FontImporter::OnImport(AssetDescriptor *ctx)
{
    const auto font = Object::Instantiate<FontData>();
    font->LoadFont(ctx->AssetPath());

    ctx->SetMainObject(font);
}
}