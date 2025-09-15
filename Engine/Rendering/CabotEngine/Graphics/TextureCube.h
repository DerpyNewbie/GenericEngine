#pragma once
#include "Texture2D.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class TextureCube final : public InspectableAsset, public ShaderResource
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_p_resource_;
    bool m_is_valid_ = false;

public:
    void OnInspectorGui() override;
    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    bool CanUpdate() override;
    bool IsValid() override;

    [[nodiscard]] ID3D12Resource *Resource() override;
    [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    bool SetTextures(std::array<AssetPtr<Texture2D>, 6> textures);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_textures_);
    }
};
}