#include "render_texture_exporter.h"

#include "Rendering/render_texture.h"

std::vector<std::string> RenderTextureExporter::SupportedExtensions()
{
    return {".rendertexture"};
}

bool RenderTextureExporter::CanExport(const std::shared_ptr<engine::Object> &object)
{
    return std::dynamic_pointer_cast<RenderTexture>(object) != nullptr;
}

void RenderTextureExporter::Export(std::ostream &output_stream, engine::AssetDescriptor *descriptor)
{
}