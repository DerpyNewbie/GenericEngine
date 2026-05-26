#include "pch.h"

#include "Texture2D.h"

#include <stb_image.h>

#include "DescriptorHeap.h"
#include <assimp/texture.h>

#include "gui.h"
#include "RenderEngine.h"
#include "Asset/asset_database.h"
#include "Rendering/texture_collection.h"

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

namespace engine
{
constexpr std::array<std::string_view, 7> kWicFormats = {".png", ".jpg", ".jpeg", ".bmp", ".dds", ".gif", ".wdp"};

Texture2D::kImageFormat Texture2D::GetImageFormat(const path &file_path)
{
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
}

void Texture2D::LoadFromAiTexture(aiTexture *ai_texture)
{
    unsigned char *pixels;
    int width = 0, height = 0, channels = 0;

    if (ai_texture->mHeight == 0)
    {
        pixels = stbi_load_from_memory(
            reinterpret_cast<const unsigned char *>(ai_texture->pcData),
            ai_texture->mWidth,
            &width,
            &height,
            &channels,
            4
        );
        m_tex_data_.reserve(width * height * sizeof(PackedVector::XMCOLOR));
    }
    else
    {
        width = ai_texture->mWidth;
        height = ai_texture->mHeight;
        pixels = reinterpret_cast<unsigned char *>(ai_texture->pcData);
    }
    m_width_ = width;
    m_height_ = height;
    m_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_tex_data_.resize(width * height);
    memcpy(m_tex_data_.data(), pixels, width * height * sizeof(PackedVector::XMCOLOR));

    if (pixels)
    {
        stbi_image_free(pixels);
    }
}

void Texture2D::LoadMetadata(const path &file_path, TexMetadata &metadata, ScratchImage &scratch)
{
    const auto format = GetImageFormat(file_path);
    HRESULT hr;

    switch (format)
    {
        case kImageFormat::kWic: {
            hr = LoadFromWICFile(file_path.c_str(), WIC_FLAGS_NONE, &metadata, scratch);
            break;
        }
        case kImageFormat::kTga: {
            hr = LoadFromTGAFile(file_path.c_str(), &metadata, scratch);
            break;
        }
        default: {
            Logger::Error<Texture2D>("Unsupported image format");
        }
    }

    if (FAILED(hr))
    {
        Logger::Error<Texture2D>("Failed load texture");
    }
}

void Texture2D::CacheData()
{
    const auto asset_descriptor = AssetDatabase::GetAssetDescriptor(Guid());
    //internalだったらキャッシュできないよ
    if (asset_descriptor == nullptr)
        return;

    const auto path = asset_descriptor->AssetPath();
    
    TexMetadata metadata;
    ScratchImage scratch;

    LoadMetadata(path, metadata, scratch);
    m_width_ = static_cast<UINT>(metadata.width);
    m_height_ = static_cast<UINT>(metadata.height);
    m_format_ = metadata.format;
    m_mip_level_ = static_cast<UINT16>(metadata.mipLevels);
}

std::vector<PackedVector::XMCOLOR> Texture2D::GetPixels()
{
    const auto asset_descriptor = AssetDatabase::GetAssetDescriptor(Guid());
    //internalなテクスチャだったらそのまま返す
    if (asset_descriptor == nullptr)
        return m_tex_data_;

    const auto path = asset_descriptor->AssetPath();

    std::vector<PackedVector::XMCOLOR> result;
    TexMetadata metadata;
    ScratchImage scratch;

    LoadMetadata(path, metadata, scratch);

    const auto img = scratch.GetImage(0, 0, 0);
    const uint8_t *src = img->pixels;
    const size_t pixel_count = img->width * img->height;

    m_width_ = static_cast<UINT>(metadata.width);
    m_height_ = static_cast<UINT>(metadata.height);
    m_format_ = metadata.format;
    m_mip_level_ = static_cast<UINT16>(metadata.mipLevels);
    result.reserve(pixel_count);

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

        result.emplace_back(color);
    }

    return result;
}

uint32_t Texture2D::Width()
{
    if (m_width_ != UINT32_MAX)
        return m_width_;

    CacheData();
    return m_width_;
}

uint32_t Texture2D::Height()
{
    if (m_height_ != UINT32_MAX)
        return m_height_;

    CacheData();
    return m_height_;
}

uint16_t Texture2D::MipLevel()
{
    if (m_mip_level_ != UINT16_MAX)
        return m_mip_level_;

    CacheData();
    return m_mip_level_;
}

DXGI_FORMAT Texture2D::Format()
{
    if (m_format_ != DXGI_FORMAT_UNKNOWN)
        return m_format_;

    CacheData();
    return m_format_;
}

Texture2D::Texture2D(uint32_t width, uint32_t height, uint16_t mip_level, DXGI_FORMAT format) : m_width_(width), m_height_(height), m_mip_level_(mip_level), m_format_(format)
{}

Texture2D::~Texture2D()
{
    TextureCollection::DeleteTexture(AssetPtr<Texture2D>::FromManaged(shared_from_base<Texture2D>()));
}

void Texture2D::OnInspectorGui()
{
    ImGui::Text("Texture2D");
    ImGui::Text("Width: %d", m_width_);
    ImGui::Text("Height: %d", m_height_);
    ImGui::Text("Mip Level: %d", m_mip_level_);

    auto texture_buffer = TextureCollection::GetTexture(shared_from_base<Texture2D>());
    if (const auto desc_heap = texture_buffer->UploadBuffer())
    {
        const auto ratio = m_height_ > 0 ? static_cast<float>(m_width_) / static_cast<float>(m_height_) : 1.0f;
        const auto max_width = ImGui::CalcItemWidth();
        static float scale = 1.0f;
        ImGui::SliderFloat("Preview Scale", &scale, 0.1f, 1.0f);
        ImGui::Image(desc_heap->handle_gpu.ptr, ImVec2(scale * max_width, scale * max_width * ratio));
        DescriptorHeap::Free(desc_heap);
    }
    else
    {
        ImGui::Text("Could not preview the texture.");
    }
}
}

CEREAL_REGISTER_TYPE(engine::Texture2D)