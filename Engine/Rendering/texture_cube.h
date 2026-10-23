#pragma once
#include "Asset/asset_ptr.h"
#include "Rendering/texture2d.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"

namespace engine
{
/// <summary>
/// 6枚のTexture2Dから作るキューブマップのAssetです（スカイボックスなどに使います）。
/// 面の順番は +X, -X, +Y, -Y, +Z, -Z で、すべて同じサイズである必要があります。
/// </summary>
class TextureCube final : public Object, public Inspectable
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;

    // GPU上のキューブマップ。初めて使う時に作る
    std::shared_ptr<CRS::Texture> m_gpu_texture_;

public:
    TextureCube(const std::array<AssetPtr<Texture2D>, 6> &textures = {});

    void OnInspectorGui() override;

    /// <summary>
    /// 6面のTextureを設定します。GPU上のキューブマップは次に使う時に作り直します。
    /// </summary>
    void SetTextures(const std::array<AssetPtr<Texture2D>, 6> &textures);

    const std::array<AssetPtr<Texture2D>, 6> &Textures() const
    {
        return m_textures_;
    }

    /// <summary>
    /// 描画に使うGPU上のキューブマップを取得します。面が揃っていない場合は無効なハンドルを返します。
    /// </summary>
    CRS::TextureHandle GpuTexture();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_textures_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TextureCube, 1)
