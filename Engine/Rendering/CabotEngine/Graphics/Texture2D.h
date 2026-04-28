#pragma once
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class Texture2DImporter;
}

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

class Texture2D : public engine::Object, public engine::Inspectable, public IBuffer, public engine::ShaderResource
{
    friend class engine::Texture2DImporter;

protected:
    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_;
    UINT m_width_ = 0;
    UINT m_height_ = 0;
    UINT16 m_mip_level_;
    DXGI_FORMAT m_format_;

    ComPtr<ID3D12Resource> m_buffer_ = nullptr;

public:
    Texture2D() = default;
    Texture2D(const aiTexture *ai_texture);

    void OnInspectorGui() override;
    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    bool CanUpdate() override;
    bool IsValid() override;

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

    [[nodiscard]] UINT Width() const
    {
        return m_width_;
    }

    [[nodiscard]] UINT Height() const
    {
        return m_height_;
    }

    [[nodiscard]] UINT16 MipLevel() const
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
            CEREAL_NVP(m_tex_data_),
            CEREAL_NVP(m_width_),
            CEREAL_NVP(m_height_),
            CEREAL_NVP(m_format_),
            CEREAL_NVP(m_mip_level_)
        );
    }
};

CEREAL_CLASS_VERSION(Texture2D, 1)