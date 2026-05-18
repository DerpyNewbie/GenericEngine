#pragma once
#include "buffer_base.h"
#include "engine_traits.h"
#include "gui.h"
#include "shader.h"
#include "texture_collection.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

enum kBufferType
{
    kBufferType_ConstantBuffer,
    kBufferType_StructuredBuffer,
    kBufferType_Texture2D
};

namespace engine
{
struct IMaterialData : Object, Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    kGpuUploadType upload_type;

    IMaterialData();
    explicit IMaterialData(ShaderParameter param);

    virtual void *Data() = 0;

    virtual int Count() = 0;
    virtual int SizeInBytes() = 0;
    virtual kBufferType BufferType() = 0;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(parameter),
            CEREAL_NVP(upload_type)
        );
    }
};

inline IMaterialData::IMaterialData() :
    parameter()
{}

inline IMaterialData::IMaterialData(ShaderParameter param) :
    parameter(std::move(param))
{}

template <typename T>
struct MaterialData : IMaterialData
{
    static constexpr bool kIsVector = engine_traits::is_vector<T>::value;
    static constexpr bool kIsTexture = std::is_same_v<TextureId, T>;

    T value;

    MaterialData();
    explicit MaterialData(const ShaderParameter &new_parameter);
    explicit MaterialData(T new_value, const ShaderParameter &new_parameter);
    ~MaterialData() override = default;

    void OnDeserialized() override;

    void OnInspectorGui() override;
    void SetValue(T value);

    void *Data() override;

    int Count() override;
    int SizeInBytes() override;
    kBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<IMaterialData>(this), CEREAL_NVP(value));
    }
};

template <typename T>
MaterialData<T>::MaterialData() :
    MaterialData({}, {})
{}

template <typename T>
MaterialData<T>::MaterialData(const ShaderParameter &new_parameter) :
    MaterialData({}, new_parameter)
{}

template <typename T>
MaterialData<T>::MaterialData(T new_value, const ShaderParameter &new_parameter) :
    IMaterialData(new_parameter), value(new_value)
{
    upload_type = kIsTexture || kIsVector ? kGpuBufferType_SRV : kGpuBufferType_CBV;
}

template <typename T>
void MaterialData<T>::OnDeserialized()
{
    is_dirty = true;
}

template <typename T>
void MaterialData<T>::OnInspectorGui()
{
    auto name = parameter.display_name.empty() ? parameter.name.c_str() : parameter.display_name.c_str();

    if constexpr (std::is_same_v<T, int>)
    {
        if (ImGui::InputInt(name, &value))
        {
            is_dirty = true;
        }
        ImGui::SetItemTooltip("Is Dirty?: %s", is_dirty ? "true" : "false");
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        if (ImGui::InputFloat(name, &value))
        {
            is_dirty = true;
        }
    }
    else if constexpr (std::is_same_v<T, Color>)
    {
        if (ImGui::CollapsingHeader("Color"))
        {
            if (Gui::PropertyField(name, value))
            {
                is_dirty = true;
            }
        }
    }
    else if constexpr (std::is_same_v<T, Vector2>)
    {
        if (Gui::PropertyField(name, value))
        {
            is_dirty = true;
        }
    }
    else if constexpr (std::is_same_v<T, Vector3>)
    {
        if (Gui::PropertyField(name, value))
        {
            is_dirty = true;
        }
    }
    else if constexpr (std::is_same_v<T, TextureId>)
    {
        auto texture = TextureCollection::GetTexture(value);
        if (Gui::PropertyField(name, texture))
        {
            value = reinterpret_cast<TextureId>(texture.Lock().get());
            is_dirty = true;
        }
    }
    else
    {
        ImGui::Text("GUI not implemented for type %s", typeid(T).name());
    }
}

template <typename T>
void MaterialData<T>::SetValue(T value)
{
    this->value = value;
    is_dirty = true;
}

template <typename T>
void *MaterialData<T>::Data()
{
    return static_cast<void *>(&value);
}

template <typename T>
int MaterialData<T>::Count()
{
    if constexpr (kIsVector)
    {
        return static_cast<int>(value.size());
    }
    else
    {
        return 1;
    }
}

template <typename T>
int MaterialData<T>::SizeInBytes()
{
    return sizeof(T) * Count();
}

template <typename T>
kBufferType MaterialData<T>::BufferType()
{
    if constexpr (std::is_same_v<T, TextureId>)
    {
        return kBufferType_Texture2D;
    }
    else if constexpr (kIsVector)
    {
        return kBufferType_StructuredBuffer;
    }
    else
    {
        return kBufferType_ConstantBuffer;
    }
}
}

CEREAL_CLASS_VERSION(engine::IMaterialData, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<bool>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<int>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<float>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<Color>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<Vector2>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<Vector3>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<engine::TextureId>, 1)