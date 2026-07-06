#pragma once
#include <assimp/texture.h>
#include "Asset/asset_ptr.h"

namespace engine
{

class Texture2D : public Object, public Inspectable
{
    friend class Texture2DImporter;

    enum class kImageFormat : uint8_t
    {
        kUnknown = 0,
        kWic,
        kTga
    };

    static kImageFormat GetImageFormat(const std::filesystem::path &file_path);

    static void LoadMetadata(const std::filesystem::path &file_path, DirectX::TexMetadata &metadata, DirectX::ScratchImage &scratch);
    void CacheData();

protected:
    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_ = {};
    uint32_t m_width_ = UINT32_MAX;
    uint32_t m_height_ = UINT32_MAX;
    uint16_t m_mip_level_ = UINT16_MAX;
    DXGI_FORMAT m_format_ = DXGI_FORMAT_UNKNOWN;

public:
    Texture2D() = default;
    Texture2D(uint32_t width, uint32_t height, uint16_t mip_level, DXGI_FORMAT format);
    ~Texture2D() override;

    void LoadFromAiTexture(aiTexture *ai_texture);
    
    void OnInspectorGui() override;

    std::vector<DirectX::PackedVector::XMCOLOR> GetPixels();

    [[nodiscard]] uint32_t Width();

    [[nodiscard]] uint32_t Height();

    [[nodiscard]] uint16_t MipLevel();
    
    [[nodiscard]] DXGI_FORMAT Format();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            cereal::make_nvp("width", m_width_),
            cereal::make_nvp("height", m_height_),
            cereal::make_nvp("format", m_format_),
            cereal::make_nvp("mip_level", m_mip_level_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Texture2D, 1)