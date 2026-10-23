#pragma once
#include "texture2d.h"

namespace engine
{
/// <summary>
/// 描画結果を書き込むためのTextureです。CameraComponentの描画先に設定でき、Materialのテクスチャとして読むこともできます。
/// </summary>
class RenderTexture : public Texture2D
{
    bool m_allow_uav_ = false;
    int m_format_index_ = 0;

protected:
    /// <summary>
    /// 描画先として使えるテクスチャを、設定されたサイズとフォーマットで作ります。
    /// </summary>
    std::shared_ptr<CRS::Texture> CreateGpuTexture() override;
    bool IsGpuTextureUpToDate() const override;

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