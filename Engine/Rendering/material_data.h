#pragma once
#include "ibuffer.h"
#include "engine_traits.h"
#include "gui.h"
#include "shader.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct IMaterialData : Object, Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    kParameterBufferType buffer_type;

    IMaterialData();
    explicit IMaterialData(ShaderParameter param);
    
    virtual void *Data() = 0;

    virtual int Count() = 0;
    virtual int SizeInBytes() = 0;
    virtual kParameterBufferType BufferType() = 0;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(parameter),
            CEREAL_NVP(buffer_type)
        );
    }
};

inline IMaterialData::IMaterialData() :
    parameter()
{ }

inline IMaterialData::IMaterialData(ShaderParameter param) :
    parameter(std::move(param))
{ }

template <typename T>
struct MaterialData : IMaterialData
{
    static constexpr bool kIsVector = engine_traits::is_vector<T>::value;
    static constexpr bool kIsAssetPtr = std::is_base_of_v<IAssetPtr, T>;
    static constexpr bool kIsTexture = std::is_same_v<AssetPtr<Texture2D>, T> || std::is_same_v<Texture2D, T>;
    
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
    kParameterBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<IMaterialData>(this), CEREAL_NVP(value));
    }
};

template <typename T>
MaterialData<T>::MaterialData() :
    MaterialData({}, {})
{ }

template <typename T>
MaterialData<T>::MaterialData(const ShaderParameter &new_parameter) :
    MaterialData({}, new_parameter)
{ }

template <typename T>
MaterialData<T>::MaterialData(T new_value, const ShaderParameter &new_parameter) :
    IMaterialData(new_parameter), value(new_value)
{
    buffer_type = kIsTexture || kIsVector ? kParameterBufferType_SRV : kParameterBufferType_CBV;
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
    else if constexpr (kIsAssetPtr)
    {
        if (Gui::PropertyField(name, value))
        {
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
kParameterBufferType MaterialData<T>::BufferType()
{
    return buffer_type;
}
}

CEREAL_CLASS_VERSION(engine::IMaterialData, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<bool>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<int>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<float>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<Color>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<Vector2>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<Vector3>, 1)

CEREAL_CLASS_VERSION(engine::MaterialData<engine::AssetPtr<Texture2D>>, 1)