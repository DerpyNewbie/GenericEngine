#pragma once
#include <assimp/texture.h>

#include "Asset/asset_ptr.h"
#include "Rendering/buffer_base.h"
#include "Rendering/shader_resource.h"

namespace engine
{
using TextureId = uint64_t;

class Texture2D : public BufferBase, public Inspectable, public ShaderResource
{
    friend class Texture2DImporter;

protected:
    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_;
    uint32_t m_width_ = 0;
    uint32_t m_height_ = 0;
    uint16_t m_mip_level_;
    DXGI_FORMAT m_format_;

    ComPtr<ID3D12Resource> m_buffer_ = nullptr;

    inline static std::unordered_map<TextureId, AssetPtr<Texture2D>> m_textures_;

public:
    static void SetTexture(const AssetPtr<Texture2D> &texture);
    static AssetPtr<Texture2D> GetTexture(TextureId texture_id);
    
    void OnInspectorGui() override;
    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    kGpuUploadType BufferType() const override
    {
        return kParameterBufferType_SRV;
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
}

CEREAL_CLASS_VERSION(engine::Texture2D, 1)