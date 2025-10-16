#include "pch.h"
#include "texture_cube_importer.h"

#include "serializer.h"
#include "Rendering/CabotEngine/Graphics/TextureCube.h"

namespace engine
{
std::vector<std::string> TextureCubeImporter::SupportedExtensions()
{
    return {".cubemap"};
}

bool TextureCubeImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<TextureCube>(object) != nullptr;
}

void TextureCubeImporter::OnImport(AssetDescriptor *ctx)
{
    Serializer serializer;
    std::ifstream is(ctx->AssetPath());
    const auto tex_cube = serializer.Load<TextureCube>(is);
    if (tex_cube == nullptr)
    {
        ctx->LogImportError("Failed to load serialized object");
        return;
    }

    ctx->SetMainObject(tex_cube);
}

void TextureCubeImporter::OnExport(AssetDescriptor *ctx)
{
    const auto texture_cube = std::dynamic_pointer_cast<TextureCube>(ctx->MainObject());
    Serializer serializer;
    std::ofstream os(ctx->AssetPath());
    serializer.Save<TextureCube>(os, texture_cube);
    os.flush();
    os.close();
}
}