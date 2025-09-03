#include "pch.h"

#include "texture_2d_importer.h"
#include "Asset/asset_database.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

using namespace DirectX;

namespace engine
{
constexpr std::array<std::string_view, 7> kWicFormats = {".png", ".jpg", ".jpeg", ".bmp", ".dds", ".gif", ".wdp"};

Texture2DImporter::kImageFormat Texture2DImporter::GetImageFormat(const path &file_path)
{
    auto ext = file_path.extension().string();
    std::ranges::transform(ext, ext.begin(), tolower);
    if (std::ranges::find(kWicFormats, ext) != kWicFormats.end())
    {
        return kImageFormat::kWic;
    }

    if (ext == ".tga")
    {
        return kImageFormat::kTga;
    }

    return kImageFormat::kUnknown;
}

IAssetPtr Texture2DImporter::GetColorTexture(PackedVector::XMCOLOR color)
{
    const auto texture_2d = Object::Instantiate<Texture2D>();
    constexpr auto width = 4;
    constexpr auto height = 4;
    constexpr auto pixel_count = width * height;

    texture_2d->width = width;
    texture_2d->height = height;
    texture_2d->format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_2d->mip_level = 1;
    texture_2d->tex_data.reserve(pixel_count);

    for (auto i = 0; i < pixel_count; ++i)
    {
        texture_2d->tex_data.emplace_back(color);
    }

    auto asset_ptr = IAssetPtr::FromManaged(texture_2d);
    return asset_ptr;
}

std::vector<std::string> Texture2DImporter::SupportedExtensions()
{
    return {".png", ".jpg", ".jpeg", ".bmp", ".dds", ".gif", ".wdp", ".tga"};
}

void Texture2DImporter::OnImport(AssetDescriptor *ctx)
{
    const auto path = ctx->AssetPath();
    const auto format = GetImageFormat(path);
    TexMetadata meta = {};
    ScratchImage scratch = {};
    HRESULT hr;

    switch (format)
    {
    case kImageFormat::kWic: {
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, scratch);
        break;
    }
    case kImageFormat::kTga: {
        hr = LoadFromTGAFile(path.c_str(), &meta, scratch);
        break;
    }
    default: {
        ctx->LogImportError("Unsupported image format");
        return;
    }
    }

    if (FAILED(hr))
    {
        ctx->LogImportError("Failed load texture");
        return;
    }

    const auto img = scratch.GetImage(0, 0, 0);
    const uint8_t *src = img->pixels;
    const size_t pixel_count = img->width * img->height;
    const auto texture_2d = Object::Instantiate<Texture2D>();

    texture_2d->width = static_cast<UINT>(meta.width);
    texture_2d->height = static_cast<UINT>(meta.height);
    texture_2d->format = meta.format;
    texture_2d->mip_level = static_cast<UINT16>(meta.mipLevels);
    texture_2d->tex_data.reserve(pixel_count);

    for (UINT i = 0; i < pixel_count; ++i)
    {
        const uint8_t r = src[i * 4 + 0];
        const uint8_t g = src[i * 4 + 1];
        const uint8_t b = src[i * 4 + 2];
        const uint8_t a = src[i * 4 + 3];

        PackedVector::XMCOLOR color;
        color.b = r;
        color.g = g;
        color.r = b;
        color.a = a;

        texture_2d->tex_data.emplace_back(color);
    }

    ctx->SetMainObject(texture_2d);
}
}