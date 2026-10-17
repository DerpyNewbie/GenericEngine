#pragma once
#include "buffer_data_base.h"

namespace engine
{
/// <summary>
/// ByteAddressBufferに渡すバイト列のデータです。
/// </summary>
class ByteAddressBufferData : public BufferDataBase
{
    size_t m_element_count_;
    std::vector<uint8_t> m_data_;

public:
    bool is_size_changed = true;

    ByteAddressBufferData() = default;
    /// <summary>
    /// Shaderのパラメーター情報からデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    ByteAddressBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    /// <summary>
    /// 要素数を設定し、データ領域をリサイズします。
    /// </summary>
    /// <param name="count">要素数</param>
    void SetCount(size_t count);
    /// <summary>
    /// 要素数分のデータをコピーして設定します。
    /// </summary>
    /// <param name="data">コピー元のデータ</param>
    void SetData(const void *data);

    /// <summary>
    /// データの先頭ポインタを取得します。
    /// </summary>
    void *Data();

    /// <summary>
    /// 要素数を取得します。
    /// </summary>
    [[nodiscard]] size_t Count() const;

    /// <summary>
    /// kBufferType_ByteAddressBufferを返します。
    /// </summary>
    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_element_count_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ByteAddressBufferData, 1)