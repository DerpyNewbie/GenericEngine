#include "pch.h"
#include "depth_texture_importer.h"

#include "serializer.h"
#include "Rendering/depth_texture.h"

std::vector<std::string> engine::DepthTextureImporter::SupportedExtensions()
{
    return {".depthtexture"};
}

bool engine::DepthTextureImporter::IsCompatibleWith(std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<DepthTexture>(object) != nullptr;
}

void engine::DepthTextureImporter::OnImport(AssetDescriptor* ctx)
{
    ctx->SetMainObject(Object::Instantiate<DepthTexture>());
}

void engine::DepthTextureImporter::OnExport(AssetDescriptor* ctx)
{
    const auto depth_texture = std::dynamic_pointer_cast<DepthTexture>(ctx->MainObject());
    if (depth_texture == nullptr)
    {
        ctx->LogImportError("This object cannot be exported with DepthTextureExporter");
    }

    std::ofstream file(ctx->AssetPath());
    Serializer serializer;
    if (!serializer.Save(file, depth_texture))
        ctx->LogImportError("Could not serialize depth texture");
}
