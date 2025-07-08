#include "pch.h"
#include "texture_2d_importer.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"
using namespace DirectX;

std::wstring FileExtension(const std::wstring &path)
{
    auto idx = path.rfind(L'.');
    return path.substr(idx + 1, path.length() - idx - 1);
}

std::vector<std::string> engine::Texture2DImporter::SupportedExtensions()
{
    return {};
}

std::shared_ptr<engine::Object> engine::Texture2DImporter::Import(AssetDescriptor *asset)
{
    auto texture_2d = Object::Instantiate<Texture2D>();
    
    const auto path = asset->path_hint;
    TexMetadata meta = {};
    ScratchImage scratch = {};
    auto ext = FileExtension(path);

    HRESULT hr = S_FALSE;
    if (ext == L"png") // use WICFile when png
    {
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, scratch);
    }
    else if (ext == L"tga") // use TGAFile when tga
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
    texture_2d->mip_levels = meta.mipLevels;

    auto img = scratch.GetImage(0, 0, 0);
    const uint8_t* src = img->pixels;
    size_t pixelCount = img->width * img->height;
    texture_2d->tex_data.reserve(pixelCount);
    for (UINT i = 0; i < pixelCount; ++i)
    {
        uint8_t r = src[i * 4 + 0];
        uint8_t g = src[i * 4 + 1];
        uint8_t b = src[i * 4 + 2];
        uint8_t a = src[i * 4 + 3];

        PackedVector::XMCOLOR color;
        color.b = b;
        color.g = g;
        color.r = r;
        color.a = a;

        texture_2d->tex_data.emplace_back(color);
    }
}