#include "pch.h"
#include "uav_texture_importer.h"
#include "serializer.h"
#include "Rendering/uav_texture.h"

std::vector<std::string> engine::UavTextureImporter::SupportedExtensions()
{
    return {".uavtexture"};
}

bool engine::UavTextureImporter::IsCompatibleWith(std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<UavTexture>(object) != nullptr;
}

void engine::UavTextureImporter::OnImport(AssetDescriptor *ctx)
{
    ctx->SetMainObject(Object::Instantiate<UavTexture>());
}

void engine::UavTextureImporter::OnExport(AssetDescriptor *ctx)
{
    const auto uav_texture = std::dynamic_pointer_cast<UavTexture>(ctx->MainObject());
    Serializer serializer;
    std::ofstream os(ctx->AssetPath());
    assert(serializer.Save<UavTexture>(os, uav_texture) && "Failed to save UavTexture");
}