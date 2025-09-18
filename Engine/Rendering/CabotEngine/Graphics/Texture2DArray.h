#pragma once
#include "Texture2D.h"
#include "Asset/asset_ptr.h"
#include "Asset/inspectable_asset.h"
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

class Texture2DArray : public IBuffer, public engine::ShaderResource
{
    std::vector<engine::AssetPtr<Texture2D>> m_textures_;
    ComPtr<ID3D12Resource> m_p_resource_;
    bool m_is_valid_ = false;

public:
    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    bool CanUpdate() override;
    bool IsValid() override;

    ID3D12Resource *Resource() override;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    void AddTexture(engine::AssetPtr<Texture2D> texture);
    void RemoveTexture(engine::AssetPtr<Texture2D> texture);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_textures_);
    }
};