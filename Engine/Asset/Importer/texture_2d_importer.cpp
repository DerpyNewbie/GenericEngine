#include "pch.h"
#include "texture_2d_importer.h"

#include "Asset/asset_database.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

#include <memory>
using namespace DirectX;

engine::IAssetPtr engine::Texture2DImporter::GetColorTexture(PackedVector::XMCOLOR color)
{
    auto texture_2d = Object::Instantiate<Texture2D>();

    texture_2d->width = 4;
    texture_2d->height = 4;
    texture_2d->format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_2d->mip_level = 1;

    size_t pixelCount = 4 * 4;
    texture_2d->tex_data.reserve(pixelCount);
    for (UINT i = 0; i < pixelCount; ++i)
    {
        texture_2d->tex_data.emplace_back(color);
    }
    auto asset_ptr = IAssetPtr::FromManaged(texture_2d);

    return asset_ptr;
}

std::vector<std::string> engine::Texture2DImporter::SupportedExtensions()
{
    return {".png", ".tga"};
}

std::shared_ptr<engine::Object> engine::Texture2DImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    auto texture_2d = Object::Instantiate<Texture2D>(asset->guid);

    const auto path = asset->path_hint;
    TexMetadata meta = {};
    ScratchImage scratch = {};
    auto ext = path.extension();

    HRESULT hr = S_FALSE;
    if (ext == L".png") // use WICFile when png
    {
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, scratch);
    }
    else if (ext == L".tga") // use TGAFile when tga
    {
        hr = LoadFromTGAFile(path.c_str(), &meta, scratch);
    }

    if (FAILED(hr))
    {
        Logger::Error<Texture2DImporter>("Failed load texture");
        return nullptr;
    }

    texture_2d->width = meta.width;
    texture_2d->height = meta.height;
    texture_2d->format = meta.format;
    texture_2d->mip_level = meta.mipLevels;

    auto img = scratch.GetImage(0, 0, 0);
    const uint8_t *src = img->pixels;
    size_t pixelCount = img->width * img->height;
    texture_2d->tex_data.reserve(pixelCount);
    for (UINT i = 0; i < pixelCount; ++i)
    {
        uint8_t r = src[i * 4 + 0];
        uint8_t g = src[i * 4 + 1];
        uint8_t b = src[i * 4 + 2];
        uint8_t a = src[i * 4 + 3];

        PackedVector::XMCOLOR color;
        color.b = r;
        color.g = g;
        color.r = b;
        color.a = a;

        texture_2d->tex_data.emplace_back(color);
    }

    return texture_2d;
}