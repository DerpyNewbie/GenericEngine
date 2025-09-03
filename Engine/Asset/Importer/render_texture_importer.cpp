#include "pch.h"
#include "render_texture_importer.h"
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
}