#pragma once
#include "buffers.h"
#include "engine_traits.h"
#include "gui.h"
#include "shader.h"
#include "Asset/asset_ptr.h"
#include "Asset/Importer/texture_2d_importer.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
struct IMaterialData : Inspectable
{
    virtual ~IMaterialData() = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer() = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual kParameterBufferType BufferType() = 0;
    virtual int SizeInBytes() = 0;
    virtual void *Data() =0;
    virtual ShaderParameter &GetShaderParam() = 0;
    virtual bool HasBufferSwapped() = 0;
};

template <typename T>
struct MaterialData : IMaterialData
{
    T value;
    std::unique_ptr<IBuffer> buffer;
    std::weak_ptr<ShaderParameter> shader_param;

    MaterialData(const std::weak_ptr<ShaderParameter> &param)
    {
        shader_param = param;
    }


    void OnInspectorGui() override
    {
        if constexpr (std::is_same_v<T, int>)
        {
            if (ImGui::InputInt(shader_param.lock()->display_name.c_str(), &value))
                UpdateBuffer();
            return;
        }

        ImGui::Text("Inspector not implemented for %s", typeid(T).name());
    }

    T Get()
    {
        return value;
    }

    void CreateBuffer() override
    {
        if constexpr (engine_traits::is_vector<T>::value)
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

    kParameterBufferType BufferType() override
    {
        if constexpr (engine_traits::is_vector<T>::value)
        {
            return kParameterBufferType_SRV;
        }
        return kParameterBufferType_CBV;
    }

    int SizeInBytes() override
    {
        if constexpr (engine_traits::is_vector<T>::value)
        {
            using ElementType = typename engine_traits::vector_element_type<T>::type;
            return static_cast<int>(value.size() * sizeof(ElementType));
        }
        else
        {
            return sizeof(T);
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

    ShaderParameter &GetShaderParam() override
    {
        return *shader_param.lock();
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

    bool HasBufferSwapped() override
    {
        return false;
    }
};

template <>
struct MaterialData<AssetPtr<Texture2D>> : IMaterialData
{
    IAssetPtr value;
    std::weak_ptr<ShaderParameter> shader_param;
    bool has_swapped = false;

    MaterialData(const std::weak_ptr<ShaderParameter> &param)
    {
        shader_param = param;
        value = Texture2DImporter::GetColorTexture(DirectX::PackedVector::XMCOLOR(1, 1, 1, 1));
    }

    void OnInspectorGui() override
    {
        if (Gui::PropertyField<Texture2D>(shader_param.lock()->display_name.c_str(), value))
        {
            if (!value.IsNull())
            {
                CreateBuffer();
                has_swapped = true;
            }
        }
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
        has_swapped = false;
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

    ShaderParameter &GetShaderParam() override
    {
        return *shader_param.lock();
    }

    bool HasBufferSwapped() override
    {
        return has_swapped;
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(value);
    }
};
}