#include "render_texture_importer.h"
#include "object.h"
#include "Rendering/render_texture.h"

std::vector<std::string> RenderTextureImporter::SupportedExtensions()
{
    return {".rendertexture"};
}

std::shared_ptr<engine::Object> RenderTextureImporter::Import(std::istream &input_stream,
                                                              engine::AssetDescriptor *asset)
{
    return engine::Object::Instantiate<RenderTexture>(asset->guid);
}