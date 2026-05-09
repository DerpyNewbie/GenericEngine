#pragma once
#include "Rendering/shader_resource.h"
#include "Rendering/BufferAsset/ibuffer.h"

namespace engine
{
class Texture2DImporter;
}

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

class Texture2D : public IBuffer, public engine::ShaderResource
{
    friend class engine::Texture2DImporter;

protected:
    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_;
    uint32_t m_width_ = 0;
    uint32_t m_height_ = 0;
    uint16_t m_mip_level_;
    DXGI_FORMAT m_format_;

    ComPtr<ID3D12Resource> m_buffer_ = nullptr;

public:
    void OnInspectorGui() override;
    bool CreateBuffer() override;
    void UpdateBuffer() override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    engine::kParameterBufferType BufferType() const override
    {
        return engine::kParameterBufferType_Texture2D;
    }
    bool IsValid() override;

    void LoadFromAiTexture(const aiTexture *ai_texture);

    ID3D12Resource *Resource() override;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    std::vector<DirectX::PackedVector::XMCOLOR> GetTexData()
    {
        return m_tex_data_;
    }

    void SetTexData(const std::vector<DirectX::PackedVector::XMCOLOR> &resource)
    {
        m_tex_data_ = resource;
    }

    [[nodiscard]] uint32_t Width() const
    {
        return m_width_;
    }

    [[nodiscard]] uint32_t Height() const
    {
        return m_height_;
    }

    [[nodiscard]] uint16_t MipLevel() const
    {
        return m_mip_level_;
    }

    [[nodiscard]] DXGI_FORMAT Format() const
    {
        return m_format_;
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            cereal::make_nvp("tex_data", m_tex_data_),
            cereal::make_nvp("width", m_width_),
            cereal::make_nvp("height", m_height_),
            cereal::make_nvp("format", m_format_),
            cereal::make_nvp("mip_level", m_mip_level_)
        );
    }
};

CEREAL_CLASS_VERSION(Texture2D, 1)