#include "pch.h"

#include "texture_2d_importer.h"
#include "Asset/asset_database.h"
#include "Rendering/texture_collection.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

using namespace DirectX;

namespace engine
{

AssetPtr<Texture2D> Texture2DImporter::GetColorTexture(PackedVector::XMCOLOR color)
{
    const auto texture_2d = Object::Instantiate<Texture2D>("Generated Color Texture");
    constexpr auto width = 4;
    constexpr auto height = 4;
    constexpr auto pixel_count = width * height;

    texture_2d->m_width_ = width;
    texture_2d->m_height_ = height;
    texture_2d->m_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_2d->m_mip_level_ = 1;
    texture_2d->m_tex_data_.reserve(pixel_count);

    for (auto i = 0; i < pixel_count; ++i)
    {
        texture_2d->m_tex_data_.emplace_back(color);
    }

    auto asset_ptr = AssetPtr<Texture2D>::FromManaged(texture_2d);
    TextureCollection::SetTexture(asset_ptr);
    return asset_ptr;
}

std::vector<std::string> Texture2DImporter::SupportedExtensions()
{
    return {".png", ".jpg", ".jpeg", ".bmp", ".dds", ".gif", ".wdp", ".tga"};
}

void Texture2DImporter::OnImport(AssetDescriptor *ctx)
{
    ctx->SetMainObject(std::make_shared<Texture2D>());
}
}