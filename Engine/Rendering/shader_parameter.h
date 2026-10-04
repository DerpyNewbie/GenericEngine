#pragma once

namespace engine
{
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

enum kBufferType
{
    kBufferType_ConstantBuffer,
    kBufferType_StructuredBuffer,
    kBufferType_Texture2D,
    kBufferType_TextureCube,
    kBufferType_UavTexture,
    kBufferType_ByteAddressBuffer,
};

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

struct ShaderParameter
{
    int index = 0;
    std::string name;
    std::string display_name;
    kBufferType buffer_type;
    bool is_unordered_access = false;
    size_t total_size = 0;
    std::vector<ShaderVariableDesc> variables = {};

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
    }
};
}

CEREAL_CLASS_VERSION(engine::ShaderParameter, 1)