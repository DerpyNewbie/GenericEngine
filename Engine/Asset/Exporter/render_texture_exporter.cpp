#include "pch.h"
#include "render_texture_exporter.h"
#include "Rendering/render_texture.h"

namespace engine
{
std::vector<std::string> RenderTextureExporter::SupportedExtensions()
{
    return {".rendertexture"};
}

bool RenderTextureExporter::CanExport(const std::shared_ptr<Object> &object)
{
    return std::dynamic_pointer_cast<RenderTexture>(object) != nullptr;
}

void RenderTextureExporter::Export(std::ostream &output_stream, AssetDescriptor *descriptor)
{}
}