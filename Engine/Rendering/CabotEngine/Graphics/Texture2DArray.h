#pragma once
#include "Texture2D.h"
#include "Asset/asset_ptr.h"
#include "Asset/inspectable_asset.h"
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

class Texture2DArray : public engine::ShaderResource
{
    std::vector<engine::AssetPtr<Texture2D>> m_textures_;
    ComPtr<ID3D12Resource> m_p_resource_;
    DXGI_FORMAT m_format_;
    UINT m_mip_level_ = 1;
    UINT16 m_element_count_ = 0;
    bool m_is_valid_ = false;

    void CopyResource();

public:
    bool CreateResource(Vector2 size, UINT16 elem_count, UINT16 mip_level = 1,
                        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
                        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
                        D3D12_CLEAR_VALUE *clear_value = nullptr);
    std::shared_ptr<DescriptorHandle> UploadBuffer();
    bool IsValid();

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