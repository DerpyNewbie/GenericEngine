#pragma once
#include "gui.h"
#include "shader.h"
#include "Asset/asset_ptr.h"
#include "BufferAsset/constant_buffer_asset.h"
#include "BufferAsset/structured_buffer_asset.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct MaterialData : Inspectable
{
    ShaderParameter parameter;
    kParameterBufferType buffer_type;
    AssetPtr<IBuffer> buffer; // can be null

    explicit MaterialData(ShaderParameter new_parameter);
    explicit MaterialData(const AssetPtr<IBuffer> &new_value, ShaderParameter new_parameter);
    ~MaterialData() override = default;

    void OnInspectorGui() override;

    template <typename T> requires kAllowedBufferType<T>
    void SetValue(T &value);
    template <typename T> requires kAllowedBufferType<T>
    void SetValue(std::vector<T> &value);

    void SetValue(const AssetPtr<IBuffer> &new_buffer);
    void SetValue(const AssetPtr<Texture2D> &new_texture);

    bool CreateBuffer() const;
    void UpdateBuffer() const;
    std::shared_ptr<DescriptorHandle> UploadBuffer() const;
    kParameterBufferType BufferType() const
    {
        return buffer_type;
    }

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(parameter),
            CEREAL_NVP(buffer_type),
            CEREAL_NVP(buffer)
        );
    }
};

template <typename T> requires kAllowedBufferType<T>
void MaterialData::SetValue(T &value)
{
    if (this->buffer != nullptr)
    {
        if (auto constant_buffer = std::dynamic_pointer_cast<ConstantBufferAsset<T>>(buffer.Lock()))
        {
            constant_buffer->SetValue(value);
        }
    }
}

template <typename T> requires kAllowedBufferType<T>
void MaterialData::SetValue(std::vector<T> &value)
{
    if (this->buffer != nullptr)
    {
        if (auto structured_buffer = std::dynamic_pointer_cast<StructuredBufferAsset<T>>(buffer.Lock()))
        {
            structured_buffer->SetData(value);
        }
    }
}
}

CEREAL_CLASS_VERSION(engine::MaterialData, 2)