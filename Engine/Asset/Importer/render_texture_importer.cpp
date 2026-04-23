#include "pch.h"
#include "render_texture_importer.h"

#include "serializer.h"
#include "Rendering/render_texture.h"

namespace engine
{
std::vector<std::string> RenderTextureImporter::SupportedExtensions()
{
    return {".rendertexture"};
}

bool RenderTextureImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<RenderTexture>(object) != nullptr;
}

void RenderTextureImporter::OnImport(AssetDescriptor *ctx)
{
    ctx->SetMainObject(Object::Instantiate<RenderTexture>());
}

void RenderTextureImporter::OnExport(AssetDescriptor *ctx)
{
    const auto render_texture = std::dynamic_pointer_cast<RenderTexture>(ctx->MainObject());
    if (render_texture == nullptr)
    {
        ctx->LogImportError("This object cannot be exported with RenderTextureExporter");
    }

    std::ofstream file(ctx->AssetPath());
    Serializer serializer;
    if (!serializer.Save(file, render_texture))
        ctx->LogImportError("Something is wrong with this render texture instance");
}
}