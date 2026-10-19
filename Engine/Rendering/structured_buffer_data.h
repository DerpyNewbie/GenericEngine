#pragma once
#include "buffer_data_base.h"

namespace engine
{
/// <summary>
/// StructuredBufferに渡すデータです。要素のサイズと要素数を持つバイト列として保持します。
/// </summary>
class StructuredBufferData : public BufferDataBase
{
    size_t m_stride_ = 4;
    size_t m_count_ = 1;

    std::vector<uint8_t> m_data_;

public:
    bool is_size_changed = true;

    /// <summary>
    /// 空のパラメーターでデータを生成します。
    /// </summary>
    StructuredBufferData();
    /// <summary>
    /// Shaderのパラメーター情報からデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    StructuredBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    /// <summary>
    /// 1要素のサイズを設定し、データ領域をリサイズします。
    /// </summary>
    /// <param name="stride">1要素のサイズ(byte)</param>
    void SetStride(size_t stride);
    /// <summary>
    /// 要素数を設定し、データ領域をリサイズします。
    /// </summary>
    /// <param name="count">要素数</param>
    void SetCount(size_t count);
    /// <summary>
    /// Size()分のデータをコピーして設定します。
    /// </summary>
    /// <param name="data">コピー元のデータ</param>
    void SetData(const void *data);

    /// <summary>
    /// データの先頭ポインタを取得します。
    /// </summary>
    void *Data();

    /// <summary>
    /// 1要素のサイズ(byte)を取得します。
    /// </summary>
    [[nodiscard]] size_t Stride() const;
    /// <summary>
    /// 要素数を取得します。
    /// </summary>
    [[nodiscard]] size_t Count() const;
    /// <summary>
    /// データ全体のサイズ(byte)を取得します。
    /// </summary>
    [[nodiscard]] size_t Size() const;

    /// <summary>
    /// kBufferType_StructuredBufferを返します。
    /// </summary>
    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_stride_),
            CEREAL_NVP(m_count_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::StructuredBufferData, 1)