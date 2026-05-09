#pragma once
#include "Texture2D.h"
#include "Asset/asset_ptr.h"
#include "Rendering/shader_resource.h"

class Texture2DArray : public engine::ShaderResource
{
    std::vector<engine::AssetPtr<Texture2D>> m_textures_;
    ComPtr<ID3D12Resource> m_buffer_;
    DXGI_FORMAT m_format_ = {};
    uint32_t m_mip_level_ = 1;
    uint16_t m_element_count_ = 0;
    bool m_is_valid_ = false;

    void CopyResource();

public:
    bool CreateResource(Vector2 size, uint16_t elem_count, uint16_t mip_level = 1,
                        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
                        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
                        D3D12_CLEAR_VALUE *clear_value = nullptr);
    std::shared_ptr<DescriptorHandle> UploadBuffer();
    bool IsValid() const;

    ID3D12Resource *Resource() override;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    void AddTexture(engine::AssetPtr<Texture2D> texture);
    void RemoveTexture(engine::AssetPtr<Texture2D> texture);

    void SetFormat(DXGI_FORMAT format);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_textures_);
    }
};