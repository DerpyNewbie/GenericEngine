#include "pch.h"
#include "depth_texture.h"

#include "application.h"
#include "gui.h"
#include "crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
std::shared_ptr<CRS::Texture> DepthTexture::CreateGpuTexture()
{
    CRS::TextureDesc desc;
    desc.width = std::max(m_width_, 1u);
    desc.height = std::max(m_height_, 1u);
    desc.format = CRS::Format::D32_Float;
    desc.usage = CRS::TextureUsage::DepthStencil | CRS::TextureUsage::ShaderResource;
    desc.debug_name = Name();
    return std::make_shared<CRS::Texture>(CrsBridge::Crs().Device(), desc);
}

bool DepthTexture::IsGpuTextureUpToDate() const
{
    const auto desc = CrsBridge::Crs().Device().GetTextureDesc(m_gpu_texture_->GetHandle());
    return desc.width == std::max(m_width_, 1u) && desc.height == std::max(m_height_, 1u);
}

void DepthTexture::OnInspectorGui()
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
}

CEREAL_REGISTER_TYPE(engine::DepthTexture)
