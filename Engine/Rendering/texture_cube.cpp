#include "pch.h"
#include "texture_cube.h"

#include "gui.h"
#include "crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
namespace
{
constexpr const char *kFaceNames[6] = {"+X (Right)", "-X (Left)", "+Y (Up)", "-Y (Down)", "+Z (Back)", "-Z (Front)"};
}

TextureCube::TextureCube(const std::array<AssetPtr<Texture2D>, 6> &textures) : m_textures_(textures)
{}

void TextureCube::OnInspectorGui()
{
    bool changed = false;
    for (int i = 0; i < 6; ++i)
        changed |= Gui::PropertyField(kFaceNames[i], m_textures_[i]);

    if (changed)
        m_gpu_texture_.reset();
}

void TextureCube::SetTextures(const std::array<AssetPtr<Texture2D>, 6> &textures)
{
    m_textures_ = textures;
    m_gpu_texture_.reset();
}

CRS::TextureHandle TextureCube::GpuTexture()
{
    if (m_gpu_texture_ != nullptr && m_gpu_texture_->IsValid())
        return m_gpu_texture_->GetHandle();

    // 6面のピクセルを、面の順番に続けて並べる
    std::vector<DirectX::PackedVector::XMCOLOR> pixels;
    uint32_t width = 0;
    uint32_t height = 0;
    for (int i = 0; i < 6; ++i)
    {
        // 参照先が見つからない（未設定、またはAssetが無い）場合も失敗にする
        const auto texture = m_textures_[i].CastedLock();
        if (texture == nullptr)
        {
            Logger::Error<TextureCube>("Texture at index %d was invalid", i);
            return {};
        }

        const auto face = texture->GetPixels();
        if (i == 0)
        {
            width = texture->Width();
            height = texture->Height();
        }
        else if (texture->Width() != width || texture->Height() != height)
        {
            Logger::Error<TextureCube>("Texture at index %d was not the same size as the first texture", i);
            return {};
        }

        pixels.insert(pixels.end(), face.begin(), face.end());
    }

    CRS::TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.dimension = CRS::TextureDimension::TextureCube;
    desc.format = CRS::Format::R8G8B8A8_UNorm;
    desc.usage = CRS::TextureUsage::ShaderResource;
    desc.initial_data = pixels.data();
    desc.initial_row_pitch = width * sizeof(DirectX::PackedVector::XMCOLOR);
    desc.debug_name = Name();
    m_gpu_texture_ = std::make_shared<CRS::Texture>(CrsBridge::Crs().Device(), desc);
    return m_gpu_texture_->GetHandle();
}
}

CEREAL_REGISTER_TYPE(engine::TextureCube)
