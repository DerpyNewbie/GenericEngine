#pragma once
#include "buffer_data_base.h"

namespace engine
{
class UavTextureBufferData : public BufferDataBase
{
    AssetPtr<RenderTexture> m_texture_;

public:
    UavTextureBufferData() = default;
    UavTextureBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void SetTexture(const AssetPtr<RenderTexture> &render_texture);
    AssetPtr<RenderTexture> Data();

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

CEREAL_CLASS_VERSION(engine::UavTextureBufferData, 1)