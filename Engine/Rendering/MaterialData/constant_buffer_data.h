#pragma once
#include "buffer_data_base.h"

namespace engine
{
/// <summary>
/// 定数バッファに渡すデータです。変数を名前で管理し、HLSLのパッキング規則(16byte境界)に従って配置します。
/// </summary>
class ConstantBufferData : public BufferDataBase
{
    size_t m_current_offset_ = 0;
    using DataTypeOffsetPair = std::pair<kConstantBufferDataType, size_t>;
    std::unordered_map<std::string, DataTypeOffsetPair> m_data_type_offset_pairs_;
    std::vector<uint8_t> m_data_;

public:
    ConstantBufferData() = default;
    /// <summary>
    /// Shaderのパラメーターに含まれる変数を順に追加してデータを生成します。
    /// </summary>
    /// <param name="shader_param">対応するShaderのパラメーター</param>
    ConstantBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    /// <summary>
    /// 変数を追加します。16byteの行をまたぐ場合は次の行の先頭にアラインします。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data_size">変数のサイズ(byte)</param>
    /// <param name="data_type">変数の型</param>
    void AddData(const std::string &name, size_t data_size, kConstantBufferDataType data_type);
    /// <summary>
    /// int型の変数を追加します。
    /// </summary>
    /// <param name="name">変数名</param>
    void AddIntData(const std::string &name);
    /// <summary>
    /// float型の変数を追加します。
    /// </summary>
    /// <param name="name">変数名</param>
    void AddFloatData(const std::string &name);
    /// <summary>
    /// Vector2型の変数を追加します。
    /// </summary>
    /// <param name="name">変数名</param>
    void AddVector2Data(const std::string &name);
    /// <summary>
    /// Vector3型の変数を追加します。
    /// </summary>
    /// <param name="name">変数名</param>
    void AddVector3Data(const std::string &name);
    /// <summary>
    /// Color型の変数を追加します。
    /// </summary>
    /// <param name="name">変数名</param>
    void AddColorData(const std::string &name);
    /// <summary>
    /// Matrix型の変数を追加します。
    /// </summary>
    /// <param name="name">変数名</param>
    void AddMatrixData(const std::string &name);

    /// <summary>
    /// int型の変数に値を設定します。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data">設定する値</param>
    /// <returns>指定した名前と型の変数が存在し、設定できた場合はtrue</returns>
    bool SetIntData(const std::string &name, int data);
    /// <summary>
    /// float型の変数に値を設定します。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data">設定する値</param>
    /// <returns>指定した名前と型の変数が存在し、設定できた場合はtrue</returns>
    bool SetFloatData(const std::string &name, float data);
    /// <summary>
    /// Vector2型の変数に値を設定します。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data">設定する値</param>
    /// <returns>指定した名前と型の変数が存在し、設定できた場合はtrue</returns>
    bool SetVector2Data(const std::string &name, Vector2 data);
    /// <summary>
    /// Vector3型の変数に値を設定します。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data">設定する値</param>
    /// <returns>指定した名前と型の変数が存在し、設定できた場合はtrue</returns>
    bool SetVector3Data(const std::string &name, Vector3 data);
    /// <summary>
    /// Color型の変数に値を設定します。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data">設定する値</param>
    /// <returns>指定した名前と型の変数が存在し、設定できた場合はtrue</returns>
    bool SetColorData(const std::string &name, Color data);
    /// <summary>
    /// Matrix型の変数に値を設定します。
    /// </summary>
    /// <param name="name">変数名</param>
    /// <param name="data">設定する値</param>
    /// <returns>指定した名前と型の変数が存在し、設定できた場合はtrue</returns>
    bool SetMatrixData(const std::string &name, const Matrix &data);

    /// <summary>
    /// データの先頭ポインタを取得します。
    /// </summary>
    void *Data();

    /// <summary>
    /// 指定した名前の変数があるかどうかを取得します。
    /// </summary>
    bool HasVariable(const std::string &name) const;
    /// <summary>
    /// 指定した名前の変数の値をコピーします。
    /// </summary>
    /// <returns>変数があり、サイズが合っていてコピーできた場合はtrue</returns>
    bool GetData(const std::string &name, void *out, size_t size) const;
    /// <summary>
    /// 指定した名前の変数に値を書き込みます。型ではなくサイズで確かめます。
    /// </summary>
    /// <returns>変数があり、サイズが合っていて書き込めた場合はtrue</returns>
    bool SetData(const std::string &name, const void *data, size_t size);

    /// <summary>
    /// 各変数を、(名前, 値の先頭, 値のサイズ)として順に渡します。
    /// </summary>
    /// <param name="action">変数ごとに呼ぶ関数</param>
    void ForEachVariable(const std::function<void(const std::string &, const void *, size_t)> &action) const;

    /// <summary>
    /// データ全体のサイズ(byte)を取得します。
    /// </summary>
    size_t Size() const;
    /// <summary>
    /// kBufferType_ConstantBufferを返します。
    /// </summary>
    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<BufferDataBase>(this),
            CEREAL_NVP(m_data_type_offset_pairs_),
            CEREAL_NVP(m_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ConstantBufferData, 1)