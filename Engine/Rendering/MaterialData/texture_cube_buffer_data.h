#pragma once
#include "buffer_data_base.h"
#include "Rendering/texture_cube.h"

namespace engine
{
/// <summary>
/// TextureCubeのスロットに渡すTextureCubeを保持するデータです。
/// </summary>
class TextureCubeBufferData : public BufferDataBase
{
    AssetPtr<TextureCube> m_texture_;

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
    /// 渡すTextureCubeを設定します。
    /// </summary>
    void SetTexture(const AssetPtr<TextureCube> &texture);
    /// <summary>
    /// 設定されているTextureCubeを取得します。
    /// </summary>
    AssetPtr<TextureCube> Data();

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<BufferDataBase>(this));

        if (version >= 2)
        {
            ar(CEREAL_NVP(m_texture_));
        }
        else
        {
            // 以前は6面のTexture2Dを持っていた。TextureCubeアセットに置き換わったので読み捨てる
            std::array<AssetPtr<Texture2D>, 6> old_textures;
            ar(cereal::make_nvp("m_textures_", old_textures));
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::TextureCubeBufferData, 2)
