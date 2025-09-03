#include "pch.h"
#include "render_texture_importer.h"
#include "Rendering/render_texture.h"

namespace engine
{
std::vector<std::string> RenderTextureImporter::SupportedExtensions()
{
    return {".rendertexture"};
}

std::shared_ptr<Object> RenderTextureImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    return Object::Instantiate<RenderTexture>(asset->guid);
}
}