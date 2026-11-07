#include "pch.h"

#include "render_texture.h"
#include "application.h"
#include "gui.h"
#include "crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace
{
constexpr const char *TextureFormatNames[] = {
    "RGBA8",
    "RGBA32"
};

constexpr DXGI_FORMAT TextureFormats[] = {
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R32G32B32A32_FLOAT
};

constexpr CRS::Format kGpuFormats[] = {
    CRS::Format::R8G8B8A8_UNorm,
    CRS::Format::R32G32B32A32_Float
};

CRS::Format ToGpuFormat(const DXGI_FORMAT format)
{
    for (int i = 0; i < IM_ARRAYSIZE(TextureFormats); ++i)
    {
        if (TextureFormats[i] == format)
            return kGpuFormats[i];
    }
    return CRS::Format::R8G8B8A8_UNorm;
}
}

namespace engine
{
RenderTexture::RenderTexture() : Texture2D()
{}

std::shared_ptr<CRS::Texture> RenderTexture::CreateGpuTexture()
{
    CRS::TextureDesc desc;
    desc.width = std::max(m_width_, 1u);
    desc.height = std::max(m_height_, 1u);
    desc.format = ToGpuFormat(m_format_);
    desc.usage = CRS::TextureUsage::RenderTarget | CRS::TextureUsage::ShaderResource;
    desc.debug_name = Name();
    return std::make_shared<CRS::Texture>(CrsBridge::Crs().Device(), desc);
}

bool RenderTexture::IsGpuTextureUpToDate() const
{
    const auto desc = CrsBridge::Crs().Device().GetTextureDesc(m_gpu_texture_->GetHandle());
    return desc.width == std::max(m_width_, 1u) && desc.height == std::max(m_height_, 1u) &&
           desc.format == ToGpuFormat(m_format_);
}

void RenderTexture::OnConstructed()
{
    m_mip_level_ = 0;
    m_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
}

void RenderTexture::OnInspectorGui()
{
    Gui::PropertyField("Width", m_width_);
    Gui::PropertyField("Height", m_height_);

    if (ImGui::Combo("Format", &m_format_index_, TextureFormatNames, IM_ARRAYSIZE(TextureFormatNames)))
        m_format_ = TextureFormats[m_format_index_];

    Gui::BoolField("Allow Uav", m_allow_uav_);
}

void RenderTexture::OnDeserialized()
{
    for (int i = 0; i < IM_ARRAYSIZE(TextureFormats); ++i)
        if (TextureFormats[i] == m_format_)
            m_format_index_ = i;
}

bool RenderTexture::AllowUav() const
{
    return m_allow_uav_;
}
}

CEREAL_REGISTER_TYPE(engine::RenderTexture)