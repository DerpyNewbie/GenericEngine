#pragma once
#include "buffers.h"
#include "engine_traits.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct IMaterialData
{
    virtual ~IMaterialData() = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer() = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual int SizeInBytes() = 0;
    virtual void *Data() =0;
};

template <typename T>
struct MaterialData : IMaterialData
{
    T value;
    std::unique_ptr<IBuffer> buffer;

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
        }
        else
        {
            buffer = std::make_unique<ConstantBuffer>(SizeInBytes());
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
}