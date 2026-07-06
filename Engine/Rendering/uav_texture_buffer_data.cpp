#include"pch.h"
#include "uav_texture_buffer_data.h"

namespace engine
{
UavTextureBufferData::UavTextureBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{}

void UavTextureBufferData::OnInspectorGui()
{
    if (Gui::PropertyField(parameter.name.c_str(), m_texture_))
    {
        SetTexture(m_texture_);
    }
}

void UavTextureBufferData::SetTexture(const AssetPtr<RenderTexture> &render_texture)
{
    if (!render_texture->AllowUav())
        m_texture_ = {};

    is_dirty = true;
}

AssetPtr<RenderTexture> UavTextureBufferData::Data()
{
    return m_texture_;
}

kBufferType UavTextureBufferData::BufferType()
{
    return kBufferType_UavTexture;
}
}

CEREAL_REGISTER_TYPE(engine::UavTextureBufferData)