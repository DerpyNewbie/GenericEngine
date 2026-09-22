#pragma once
#include "buffer_data_base.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class TextureCubeBufferData : public BufferDataBase
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;

public:
    TextureCubeBufferData() = default;
    TextureCubeBufferData(const ShaderParameter& shader_param);

    void OnInspectorGui() override;
    kBufferType BufferType() override;
    std::array<AssetPtr<Texture2D>, 6>& Textures();
    
    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_textures_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TextureCubeBufferData, 1)