#pragma once
#include "buffer_data_base.h"

namespace engine
{
/// <summary>
/// サンプラー（テクスチャの補間方法と、UVが0..1の外に出たときの扱い）の設定です。
/// </summary>
class SamplerData : public BufferDataBase
{
    // 0: Point, 1: Linear, 2: Anisotropic（CRS::Filterと同じ並び）
    int m_filter_ = 1;
    // 0: Wrap, 1: Clamp, 2: Mirror（CRS::AddressModeと同じ並び）
    int m_address_ = 0;

public:
    SamplerData() = default;
    /// <summary>
    /// Shaderのパラメーター情報からデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    SamplerData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    /// <summary>
    /// 補間方法とUVの扱いを設定します。
    /// </summary>
    /// <param name="filter">0: Point, 1: Linear, 2: Anisotropic</param>
    /// <param name="address">0: Wrap, 1: Clamp, 2: Mirror</param>
    void SetState(int filter, int address);
    /// <summary>
    /// 補間方法です（0: Point, 1: Linear, 2: Anisotropic）。
    /// </summary>
    [[nodiscard]] int Filter() const;
    /// <summary>
    /// UVの扱いです（0: Wrap, 1: Clamp, 2: Mirror）。
    /// </summary>
    [[nodiscard]] int Address() const;

    /// <summary>
    /// kBufferType_Samplerを返します。
    /// </summary>
    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_filter_),
            CEREAL_NVP(m_address_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::SamplerData, 1)