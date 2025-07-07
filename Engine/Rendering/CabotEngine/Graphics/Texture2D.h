#pragma once
#include "Asset/inspectable_asset.h"
#include "Rendering/buffers.h"

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

class Texture2D final : public engine::InspectableAsset, public IBuffer
{
public:
    std::vector<DirectX::PackedVector::XMCOLOR> tex_data;
    UINT width = 0;
    UINT height = 0;
    UINT16 mip_levels;
    DXGI_FORMAT format;
    void OnInspectorGui() override;

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    bool CanUpdate() override;
    bool IsValid() override;

    ID3D12Resource *Resource();
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this), tex_data, width, height, format, mip_levels);
    }
private:
    ComPtr<ID3D12Resource> m_pResource;
    bool m_IsValid = false;
};