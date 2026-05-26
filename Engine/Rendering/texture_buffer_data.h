#pragma once
#include "buffer_data_base.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class TextureBufferData : public BufferDataBase
{
    AssetPtr<Texture2D> m_texture_;

public:
    TextureBufferData() = default;
    TextureBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void SetTexture(const AssetPtr<Texture2D> &texture);
    AssetPtr<Texture2D> Data();

    kBufferType BufferType() override;
    
    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_texture_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TextureBufferData, 1)