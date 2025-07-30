#pragma once
#include "buffers.h"
#include "engine_traits.h"
#include "gui.h"
#include "shader.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct IMaterialData : Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    std::shared_ptr<IBuffer> buffer = nullptr; // can be null

    IMaterialData();
    explicit IMaterialData(ShaderParameter param);

    virtual std::shared_ptr<IBuffer> CreateBuffer() = 0;
    virtual bool CanUpdateBuffer() = 0;
    virtual void UpdateBuffer() = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;

    virtual void *Data() = 0;

    virtual int Count() = 0;
    virtual int SizeInBytes() = 0;
    virtual kParameterBufferType BufferType() = 0;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(parameter);
    }
};

inline IMaterialData::IMaterialData() : parameter()
{
}

inline IMaterialData::IMaterialData(ShaderParameter param): parameter(std::move(param))
{
}

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

    MaterialData();
    explicit MaterialData(const ShaderParameter &new_parameter);
    explicit MaterialData(T new_value, const ShaderParameter &new_parameter);
    ~MaterialData() override = default;

    void OnInspectorGui() override;

    std::shared_ptr<IBuffer> CreateBuffer() override;
    bool CanUpdateBuffer() override;
    void UpdateBuffer() override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;

    void *Data() override;

    int Count() override;
    int SizeInBytes() override;
    kParameterBufferType BufferType() override;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<IMaterialData>(this), CEREAL_NVP(value));
    }
};

template <typename T>
MaterialData<T>::MaterialData() : MaterialData({}, {})
{
}

template <typename T>
MaterialData<T>::MaterialData(const ShaderParameter &new_parameter) : MaterialData({}, new_parameter)
{
}

template <typename T>
MaterialData<T>::MaterialData(T new_value, const ShaderParameter &new_parameter) :
    IMaterialData(new_parameter), value(new_value)
{
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
            return value.CastedLock()->GetTexData().data();
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
        return value.size();
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