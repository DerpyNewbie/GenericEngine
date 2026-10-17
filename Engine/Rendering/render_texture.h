#pragma once
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
/// <summary>
/// 描画結果を書き込むためのTextureです。UAVを許可するとComputeShaderなどから書き込むこともできます。
/// </summary>
class RenderTexture : public Texture2D
{
    friend class RenderTextureBuffer;

    bool m_allow_uav_;
    int m_format_index_;
    
public:
    RenderTexture();

    void OnConstructed() override;
    void OnInspectorGui() override;
    void OnDeserialized() override;

    /// <summary>
    /// UAVとして使用可能かどうかを取得します。
    /// </summary>
    bool AllowUav() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Texture2D>(this),
            CEREAL_NVP(m_allow_uav_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RenderTexture, 1)