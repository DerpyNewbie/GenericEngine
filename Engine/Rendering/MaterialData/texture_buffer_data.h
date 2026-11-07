#pragma once
#include "buffer_data_base.h"
#include "Asset/asset_ptr.h"
#include "Rendering/texture2d.h"

namespace engine
{
/// <summary>
/// Texture2Dのスロットに渡すTextureを保持するデータです。
/// </summary>
class TextureBufferData : public BufferDataBase
{
    AssetPtr<Texture2D> m_texture_;

public:
    TextureBufferData() = default;
    /// <summary>
    /// Shaderのパラメーター情報からデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    TextureBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    /// <summary>
    /// 渡すTextureを設定します。
    /// </summary>
    /// <param name="texture">設定するTexture</param>
    void SetTexture(const AssetPtr<Texture2D> &texture);
    /// <summary>
    /// 設定されているTextureを取得します。
    /// </summary>
    AssetPtr<Texture2D> Data();

    /// <summary>
    /// kBufferType_Texture2Dを返します。
    /// </summary>
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