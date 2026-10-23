#pragma once
#include "texture2d.h"

namespace engine
{
/// <summary>
/// 深度値を書き込むためのTextureです。CameraComponentの深度の描画先に設定できます。
/// </summary>
class DepthTexture : public Texture2D
{
protected:
    /// <summary>
    /// 深度の描画先として使えるテクスチャ(D32)を、設定されたサイズで作ります。
    /// </summary>
    std::shared_ptr<CRS::Texture> CreateGpuTexture() override;
    /// <summary>
    /// GPU上のテクスチャが、今のサイズと合っているかどうかです。
    /// </summary>
    bool IsGpuTextureUpToDate() const override;

public:
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Texture2D>(this)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::DepthTexture, 1)