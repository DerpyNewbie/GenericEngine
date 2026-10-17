#pragma once
#include "buffer_data_base.h"
#include "render_texture.h"
#include "Asset/asset_ptr.h"

namespace engine
{
/// <summary>
/// RWTexture2DのスロットにUAVとして渡すRenderTextureを保持するデータです。
/// </summary>
class UavTextureBufferData : public BufferDataBase
{
    AssetPtr<RenderTexture> m_texture_;

public:
    UavTextureBufferData() = default;
    /// <summary>
    /// Shaderのパラメーター情報からデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    UavTextureBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    /// <summary>
    /// 渡すRenderTextureを設定します。UAVを許可していないRenderTextureの場合は設定を解除します。
    /// </summary>
    /// <param name="render_texture">設定するRenderTexture</param>
    void SetTexture(const AssetPtr<RenderTexture> &render_texture);
    /// <summary>
    /// 設定されているRenderTextureを取得します。
    /// </summary>
    AssetPtr<RenderTexture> Data();

    /// <summary>
    /// kBufferType_UavTextureを返します。
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

CEREAL_CLASS_VERSION(engine::UavTextureBufferData, 1)