#include "pch.h"
#include "depth_texture.h"

#include "application.h"
#include "gui.h"
#include "CabotEngine/Graphics/DirectXResourceFactory.h"
#include "CabotEngine/Graphics/RenderEngine.h"

CEREAL_REGISTER_TYPE(engine::DepthTexture)

void engine::DepthTexture::OnInspectorGui()
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
}
