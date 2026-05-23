#include "pch.h"
#include "texture_buffer_data.h"

namespace engine
{
TextureBufferData::TextureBufferData(const ShaderParameter &shader_param) : MaterialDataBase(shader_param)
{}

void TextureBufferData::OnInspectorGui()
{
    if (Gui::PropertyField("Texture", m_texture_))
        SetTexture(m_texture_);
}

void TextureBufferData::SetTexture(const AssetPtr<Texture2D> &texture)
{
    m_texture_ = texture;

    is_dirty = true;
}

AssetPtr<Texture2D> TextureBufferData::Data()
{
    return m_texture_;
}
}

CEREAL_REGISTER_TYPE(engine::TextureBufferData)