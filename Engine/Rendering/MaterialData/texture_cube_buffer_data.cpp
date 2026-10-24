#include "pch.h"
#include "texture_cube_buffer_data.h"

#include "gui.h"

namespace engine
{
TextureCubeBufferData::TextureCubeBufferData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{}

void TextureCubeBufferData::OnInspectorGui()
{
    if (Gui::PropertyField(parameter.name.c_str(), m_texture_))
        is_dirty = true;
}

kBufferType TextureCubeBufferData::BufferType()
{
    return kBufferType_TextureCube;
}

void TextureCubeBufferData::SetTexture(const AssetPtr<TextureCube> &texture)
{
    m_texture_ = texture;
    is_dirty = true;
}

AssetPtr<TextureCube> TextureCubeBufferData::Data()
{
    return m_texture_;
}
}

CEREAL_REGISTER_TYPE(engine::TextureCubeBufferData)
