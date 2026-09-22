#include "pch.h"
#include "gui.h"
#include "texture_cube_buffer_data.h"

namespace engine
{
TextureCubeBufferData::TextureCubeBufferData(const ShaderParameter& shader_param):
    BufferDataBase(shader_param)
{
}

void TextureCubeBufferData::OnInspectorGui()
{
    for (int i = 0; i < 6; ++i)
    {
        constexpr const char* dir_labels[] = {"Right", "Left", "Bottom", "Top", "Front", "Back"};
        if (Gui::PropertyField(dir_labels[i], m_textures_[i]))
            is_dirty = true;
    }
}

kBufferType TextureCubeBufferData::BufferType()
{
    return kBufferType_TextureCube;
}

std::array<AssetPtr<Texture2D>, 6>& TextureCubeBufferData::Textures()
{
    return m_textures_;
}
}

CEREAL_REGISTER_TYPE(engine::TextureCubeBufferData)
