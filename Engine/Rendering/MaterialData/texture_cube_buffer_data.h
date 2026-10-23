#pragma once
#include "buffer_data_base.h"
#include "Rendering/texture2d.h"

namespace engine
{
/// <summary>
/// TextureCubeのスロットに渡す6面分のTextureを保持するデータです。
/// </summary>
class TextureCubeBufferData : public BufferDataBase
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;

public:
    TextureCubeBufferData() = default;
    /// <summary>
    /// Shaderのパラメーター情報からデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    TextureCubeBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;
    /// <summary>
    /// kBufferType_TextureCubeを返します。
    /// </summary>
    kBufferType BufferType() override;
    /// <summary>
    /// 6面分のTextureの参照を取得します。順番はRight、Left、Bottom、Top、Front、Backです。
    /// </summary>
    std::array<AssetPtr<Texture2D>, 6> &Textures();

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