#include "pch.h"

#include "render_texture.h"
#include "application.h"
#include "gui.h"

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
}

namespace engine
{
RenderTexture::RenderTexture() : Texture2D()
{
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