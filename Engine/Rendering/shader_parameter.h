#pragma once

namespace engine
{
/// <summary>
/// 定数バッファ内の変数の型です。
/// </summary>
enum class kConstantBufferDataType
{
    kConstantBufferDataTypeInt,
    kConstantBufferDataType_Float,
    kConstantBufferDataType_Vector2,
    kConstantBufferDataType_Vector3,
    kConstantBufferDataType_Color,
    kConstantBufferDataType_Matrix,
    kConstantBufferDataType_Unknown
};

/// <summary>
/// Shaderのパラメーターの種類です。
/// </summary>
enum kBufferType
{
    kBufferType_ConstantBuffer,
    kBufferType_StructuredBuffer,
    kBufferType_Texture2D,
    kBufferType_TextureCube,
    kBufferType_UavTexture,
    kBufferType_ByteAddressBuffer,
    kBufferType_Sampler,
};

/// <summary>
/// 定数バッファ内の1つの変数の名前と型です。
/// </summary>
struct ShaderVariableDesc
{
    std::string name;
    kConstantBufferDataType data_type;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(name),
            CEREAL_NVP(data_type)
        );
    }
};

/// <summary>
/// Shaderのリフレクションから取得した1つのパラメーター(Buffer/Texture)の情報です。
/// </summary>
struct ShaderParameter
{
    int index = 0;
    std::string name;
    std::string display_name;
    kBufferType buffer_type = kBufferType_ConstantBuffer;
    bool is_unordered_access = false;
    size_t total_size = 0;
    std::vector<ShaderVariableDesc> variables = {};

    /// <summary>
    /// indexとnameが一致するかどうかで比較します。
    /// </summary>
    bool operator==(const ShaderParameter &other) const
    {
        // do not check for `display_name` or `type_hint` because only real parameters are `index` and `name`
        return index == other.index && name == other.name;
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(index),
            CEREAL_NVP(name),
            CEREAL_NVP(display_name),
            CEREAL_NVP(is_unordered_access),
            CEREAL_NVP(total_size),
            CEREAL_NVP(variables)
        );

        if (version >= 2)
        {
            ar(CEREAL_NVP(buffer_type));
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::ShaderParameter, 2)