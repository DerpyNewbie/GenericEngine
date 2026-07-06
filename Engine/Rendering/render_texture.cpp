#include "pch.h"

#include "render_texture.h"
#include "application.h"
#include "gui.h"

namespace engine
{
RenderTexture::RenderTexture() : Texture2D(Application::WindowWidth(), Application::WindowHeight(), 0, DXGI_FORMAT_R8G8B8A8_UNORM)
{}

void RenderTexture::OnInspectorGui()
{
    int width = m_width_;
    if (Gui::PropertyField("Width", width))
    {
        m_width_ = width;
    }

    int height = m_height_;
    if (Gui::PropertyField("Height", height))
    {
        m_height_ = height;
    }

    Gui::BoolField("Allow Uav", m_allow_uav_);
}

bool RenderTexture::AllowUav() const
{
    return m_allow_uav_;
}
}

CEREAL_REGISTER_TYPE(engine::RenderTexture)