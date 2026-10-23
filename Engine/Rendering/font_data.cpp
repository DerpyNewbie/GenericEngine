#include "pch.h"
#include "font_data.h"
#include "crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
namespace
{
constexpr char kSpriteFontMagic[] = "DXTKfont";

/// <summary>
/// バイト列を先頭から順に読みます。
/// </summary>
class BinaryReader
{
    const std::vector<uint8_t> &m_data_;
    size_t m_offset_ = 0;

public:
    explicit BinaryReader(const std::vector<uint8_t> &data) : m_data_(data)
    {}

    template <typename T>
    bool Read(T &out)
    {
        if (m_offset_ + sizeof(T) > m_data_.size())
            return false;

        std::memcpy(&out, m_data_.data() + m_offset_, sizeof(T));
        m_offset_ += sizeof(T);
        return true;
    }

    bool ReadBytes(std::vector<uint8_t> &out, const size_t size)
    {
        if (m_offset_ + size > m_data_.size())
            return false;

        out.assign(m_data_.begin() + m_offset_, m_data_.begin() + m_offset_ + size);
        m_offset_ += size;
        return true;
    }
};
}

bool FontData::LoadFont(const std::wstring &font_path)
{
    std::ifstream file(font_path, std::ios::binary);
    if (!file)
    {
        Logger::Error<FontData>("Failed to open font file");
        return false;
    }
    const std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    BinaryReader reader(data);

    char magic[sizeof(kSpriteFontMagic) - 1];
    if (!reader.Read(magic) || std::memcmp(magic, kSpriteFontMagic, sizeof(magic)) != 0)
    {
        Logger::Error<FontData>("Not a spritefont file");
        return false;
    }

    uint32_t glyph_count = 0;
    if (!reader.Read(glyph_count))
        return false;

    m_glyphs_.resize(glyph_count);
    for (auto &glyph : m_glyphs_)
    {
        if (!reader.Read(glyph.character) || !reader.Read(glyph.left) || !reader.Read(glyph.top) ||
            !reader.Read(glyph.right) || !reader.Read(glyph.bottom) || !reader.Read(glyph.x_offset) ||
            !reader.Read(glyph.y_offset) || !reader.Read(glyph.x_advance))
        {
            Logger::Error<FontData>("Corrupted spritefont glyphs");
            return false;
        }
    }
    std::ranges::sort(m_glyphs_, {}, &Glyph::character);

    if (!reader.Read(m_line_spacing_) || !reader.Read(m_default_character_) ||
        !reader.Read(m_texture_.width) || !reader.Read(m_texture_.height) || !reader.Read(m_texture_.format) ||
        !reader.Read(m_texture_.row_pitch) || !reader.Read(m_texture_.row_count) ||
        !reader.ReadBytes(m_texture_.pixels, static_cast<size_t>(m_texture_.row_pitch) * m_texture_.row_count))
    {
        Logger::Error<FontData>("Corrupted spritefont texture");
        return false;
    }

    return true;
}

CRS::TextureHandle FontData::GpuTexture()
{
    if (m_gpu_texture_ != nullptr && m_gpu_texture_->IsValid())
        return m_gpu_texture_->GetHandle();

    if (m_texture_.pixels.empty())
        return {};

    // spritefontの画像はRGBA8か、ブロック圧縮(BC1 / BC2 / BC3)
    CRS::Format format;
    switch (m_texture_.format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        format = CRS::Format::R8G8B8A8_UNorm;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        format = CRS::Format::B8G8R8A8_UNorm;
        break;
    case DXGI_FORMAT_BC1_UNORM:
        format = CRS::Format::BC1_UNorm;
        break;
    case DXGI_FORMAT_BC2_UNORM:
        format = CRS::Format::BC2_UNorm;
        break;
    case DXGI_FORMAT_BC3_UNORM:
        format = CRS::Format::BC3_UNorm;
        break;
    default:
        Logger::Error<FontData>("Unsupported font texture format: %u", m_texture_.format);
        return {};
    }

    CRS::TextureDesc desc;
    desc.width = m_texture_.width;
    desc.height = m_texture_.height;
    desc.format = format;
    desc.usage = CRS::TextureUsage::ShaderResource;
    desc.initial_data = m_texture_.pixels.data();
    desc.initial_row_pitch = m_texture_.row_pitch;
    desc.debug_name = Name();
    m_gpu_texture_ = std::make_shared<CRS::Texture>(CrsBridge::Crs().Device(), desc);
    return m_gpu_texture_->GetHandle();
}

const FontData::Glyph *FontData::FindGlyph(const uint32_t character) const
{
    const auto find = [&](const uint32_t c) -> const Glyph * {
        const auto itr = std::ranges::lower_bound(m_glyphs_, c, {}, &Glyph::character);
        return itr != m_glyphs_.end() && itr->character == c ? &*itr : nullptr;
    };

    if (const auto glyph = find(character))
        return glyph;

    return m_default_character_ != 0 ? find(m_default_character_) : nullptr;
}

void FontData::ForEachGlyph(const std::string &string,
                            const std::function<void(const Glyph &, Vector2)> &action) const
{
    // DirectXTKのSpriteFontと同じ並べ方
    float x = 0;
    float y = 0;
    for (const auto c : string)
    {
        if (c == '\r')
            continue;

        if (c == '\n')
        {
            x = 0;
            y += m_line_spacing_;
            continue;
        }

        const auto glyph = FindGlyph(static_cast<unsigned char>(c));
        if (glyph == nullptr)
            continue;

        x = std::max(x + glyph->x_offset, 0.0f);

        const auto width = static_cast<float>(glyph->right - glyph->left);
        const auto height = static_cast<float>(glyph->bottom - glyph->top);
        if (!std::isspace(static_cast<unsigned char>(c)) || width > 1 || height > 1)
            action(*glyph, Vector2(x, y + glyph->y_offset));

        x += width + glyph->x_advance;
    }
}
}
