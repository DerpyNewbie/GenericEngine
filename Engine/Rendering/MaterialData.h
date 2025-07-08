#pragma once
#include "buffers.h"
#include "engine_traits.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct IMaterialData
{
    virtual ~IMaterialData() = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer() = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual kParameterBufferType BufferType() = 0;
    virtual int SizeInBytes() = 0;
    virtual void *Data() =0;
};

template <typename T>
struct MaterialData : IMaterialData
{
    T value;
    std::unique_ptr<IBuffer> buffer;
    kParameterBufferType buffer_type;

    MaterialData() = default;

    void Set(T new_value)
    {
        value = new_value;
    }

    T Get()
    {
        return value;
    }

    void CreateBuffer() override
    {
        if constexpr (!engine_traits::is_vector<T>::value)
        {
            using ElementType = typename engine_traits::vector_element_type<T>::type;
            buffer = std::make_unique<StructuredBuffer>(sizeof(ElementType), Count());
            buffer_type = kParameterBufferType_SRV;
        }
        else
        {
            buffer = std::make_unique<ConstantBuffer>(SizeInBytes());
            buffer_type = kParameterBufferType_CBV;
        }
        buffer->CreateBuffer();
    }

    void UpdateBuffer() override
    {
        if (buffer->CanUpdate())
        {
            buffer->UpdateBuffer(Data());
        }
    }

    std::shared_ptr<DescriptorHandle> UploadBuffer()
    {
        return buffer->UploadBuffer();
    }
    kParameterBufferType BufferType() override
    {
        return buffer_type;
    }

    int SizeInBytes() override
    {
        if constexpr (!engine_traits::is_vector<T>::value)
        {
            return sizeof(T);
        }
        else
        {
            using ElementType = typename engine_traits::vector_element_type<T>::type;
            return static_cast<int>(value.size() * sizeof(ElementType));
        }
    }

    size_t Count()
    {
        if constexpr (engine_traits::is_vector<T>::value) // T が vector のときだけ
        {
            return value.size();
        }
        else // それ以外（スカラ型など）
        {
            return 1;
        }
    }

    void *Data() override
    {
        if constexpr (!engine_traits::is_vector<T>::value)
        {
            return &value;
        }
        else
        {
            return value.data();
        }
    }
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(value);
        if (!buffer)
        {
            CreateBuffer();
        }
    }
};

template <>
struct MaterialData<IAssetPtr> : IMaterialData
{
    IAssetPtr value;
    MaterialData() = default;

    void Set(AssetPtr<Texture2D> new_value)
    {
        value = static_cast<IAssetPtr>(new_value);
    }

    std::shared_ptr<Texture2D> Get()
    {
        return std::dynamic_pointer_cast<Texture2D>(value.Lock());
    }

    void CreateBuffer() override
    {
        std::dynamic_pointer_cast<Texture2D>(value.Lock())->CreateBuffer();
    }

    void UpdateBuffer() override
    {
    }

    std::shared_ptr<DescriptorHandle> UploadBuffer() override
    {
        return std::dynamic_pointer_cast<Texture2D>(value.Lock())->UploadBuffer();
    }

    kParameterBufferType BufferType() override
    {
        return kParameterBufferType_SRV;
    }

    int SizeInBytes() override
    {
        return sizeof(Texture2D);
    }

    void *Data() override
    {
        return &std::dynamic_pointer_cast<Texture2D>(value.Lock())->tex_data;
    }
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(value);
    }
};
}