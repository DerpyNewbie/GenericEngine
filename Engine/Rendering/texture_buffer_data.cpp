#include "pch.h"
#include "texture_buffer_data.h"

#include "Asset/Importer/texture_2d_importer.h"

namespace engine
{
TextureBufferData::TextureBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{
}

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

kBufferType TextureBufferData::BufferType()
{
    return kBufferType_Texture2D;
}
}

CEREAL_REGISTER_TYPE(engine::TextureBufferData)