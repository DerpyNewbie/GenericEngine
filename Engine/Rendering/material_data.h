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
/// <summary>
/// Materialが持つ1つのShaderParameterの値のInterfaceです。
/// </summary>
struct IMaterialData : Object, Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    std::shared_ptr<IBuffer> buffer = nullptr; // can be null

    /// <summary>
    /// 空のparameterでデータを作成します。
    /// </summary>
    IMaterialData();
    /// <summary>
    /// 対応するShaderのparameterを指定してデータを作成します。
    /// </summary>
    /// <param name="param">対応するShaderのparameter</param>
    explicit IMaterialData(ShaderParameter param);

    /// <summary>
    /// 値の型に合ったBufferを作成します。
    /// </summary>
    virtual std::shared_ptr<IBuffer> CreateBuffer() = 0;
    /// <summary>
    /// Bufferの内容を書き換えられるかどうかを取得します。
    /// </summary>
    virtual bool CanUpdateBuffer() = 0;
    /// <summary>
    /// 値が変更されている場合に、Bufferへ書き込みます。
    /// </summary>
    virtual void UpdateBuffer() = 0;
    /// <summary>
    /// BufferをDescriptorHeapに登録します。Bufferがない場合は作成します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;

    /// <summary>
    /// Bufferに書き込む値のデータへのポインタを取得します。
    /// </summary>
    virtual void *Data() = 0;

    /// <summary>
    /// 値の要素数を取得します。
    /// </summary>
    virtual int Count() = 0;
    /// <summary>
    /// 値全体のサイズ(byte)を取得します。
    /// </summary>
    virtual int SizeInBytes() = 0;
    /// <summary>
    /// 値がCBV、SRV、UAVのどれとして扱われるかを取得します。
    /// </summary>
    virtual kParameterBufferType BufferType() = 0;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(parameter)
            );
    }
};

inline IMaterialData::IMaterialData() :
    parameter()
{ }

inline IMaterialData::IMaterialData(ShaderParameter param) :
    parameter(std::move(param))
{ }

/// <summary>
/// T型の値を持つMaterialDataです。
/// </summary>
template <typename T>
struct MaterialData : IMaterialData
{
    static constexpr bool kIsVector = engine_traits::is_vector<T>::value;
    static constexpr bool kIsAssetPtr = std::is_base_of_v<IAssetPtr, T>;
    static constexpr bool kIsTexture = std::is_same_v<AssetPtr<Texture2D>, T> || std::is_same_v<Texture2D, T>;
    static constexpr kParameterBufferType kBufferType = kIsVector || kIsTexture
    ? kParameterBufferType_SRV
    : kParameterBufferType_CBV;

    T value;

    /// <summary>
    /// 既定値と空のparameterでデータを作成します。
    /// </summary>
    MaterialData();
    /// <summary>
    /// 既定値でデータを作成します。
    /// </summary>
    /// <param name="new_parameter">対応するShaderのparameter</param>
    explicit MaterialData(const ShaderParameter &new_parameter);
    /// <summary>
    /// 値とparameterを指定してデータを作成します。
    /// </summary>
    /// <param name="new_value">初期値</param>
    /// <param name="new_parameter">対応するShaderのparameter</param>
    explicit MaterialData(T new_value, const ShaderParameter &new_parameter);
    ~MaterialData() override = default;

    /// <summary>
    /// Bufferへの書き込みが必要な状態にし、値がAssetの場合はBufferを作成します。
    /// </summary>
    void OnDeserialized() override;

    void OnInspectorGui() override;
    /// <summary>
    /// 値を設定し、Bufferへの書き込みが必要な状態にします。Textureの場合はBufferを作り直します。
    /// </summary>
    /// <param name="value">設定する値</param>
    void SetValue(T value);

    /// <summary>
    /// Textureの場合はTexture自身を、それ以外の場合はConstantBufferまたはStructuredBufferを作成します。
    /// </summary>
    std::shared_ptr<IBuffer> CreateBuffer() override;
    /// <summary>
    /// Bufferがあり、内容を書き換えられるかどうかを取得します。
    /// </summary>
    bool CanUpdateBuffer() override;
    /// <summary>
    /// 値が変更されている場合に、Bufferへ書き込みます。Bufferがない場合は作成します。
    /// </summary>
    void UpdateBuffer() override;
    /// <summary>
    /// BufferをDescriptorHeapに登録します。Bufferがない場合は作成します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;

    /// <summary>
    /// Bufferに書き込む値のデータへのポインタを取得します。
    /// </summary>
    void *Data() override;

    /// <summary>
    /// 値の要素数を取得します。std::vectorの場合はその要素数、それ以外の場合は 1 を返します。
    /// </summary>
    int Count() override;
    /// <summary>
    /// 値全体のサイズ(byte)を取得します。
    /// </summary>
    int SizeInBytes() override;
    /// <summary>
    /// 値の型に応じて、CBVとSRVのどちらとして扱われるかを取得します。
    /// </summary>
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
{ }

template <typename T>
void MaterialData<T>::OnDeserialized()
{
    is_dirty = true;
    if constexpr (kIsAssetPtr)
        if (value.Lock())
            CreateBuffer();
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
            buffer = CreateBuffer();
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
    if constexpr (kIsTexture)
    {
        buffer = CreateBuffer();
    }
}

template <typename T>
std::shared_ptr<IBuffer> MaterialData<T>::CreateBuffer()
{
    std::shared_ptr<IBuffer> buff = nullptr;
    if constexpr (kIsTexture)
    {
        if constexpr (kIsAssetPtr)
        {
            buff = value.CastedLock();
        }
        else
        {
            buff = value;
        }
    }
    else if constexpr (kBufferType == kParameterBufferType_CBV)
    {
        buff = std::make_shared<ConstantBuffer>(SizeInBytes());
    }
    else if constexpr (kBufferType == kParameterBufferType_SRV)
    {
        buff = std::make_shared<StructuredBuffer>(SizeInBytes(), Count());
    }
    else
    {
        static_assert("Not Implemented");
        return nullptr;
    }

    buff->CreateBuffer();
    return buff;
}

template <typename T>
bool MaterialData<T>::CanUpdateBuffer()
{
    return buffer != nullptr && buffer->CanUpdate();
}

template <typename T>
void MaterialData<T>::UpdateBuffer()
{
    if (buffer == nullptr)
    {
        buffer = CreateBuffer();
    }

    if (is_dirty && buffer->CanUpdate())
    {
        Logger::Log<MaterialData>("Updating buffer: %s", parameter.name.c_str());
        buffer->UpdateBuffer(Data());
        is_dirty = false;
    }
}

template <typename T>
std::shared_ptr<DescriptorHandle> MaterialData<T>::UploadBuffer()
{
    if (buffer == nullptr)
    {
        buffer = CreateBuffer();
    }

    return buffer->UploadBuffer();
}

template <typename T>
void *MaterialData<T>::Data()
{
    if constexpr (kIsTexture)
    {
        if constexpr (kIsAssetPtr)
        {
            return value->GetTexData().data();
        }
        else
        {
            return value->tex_data;
        }
    }
    else if constexpr (kIsVector)
    {
        return value.data();
    }
    else if constexpr (kIsAssetPtr)
    {
        return value.Lock().get();
    }
    else
    {
        return &value;
    }
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
    return kBufferType;
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