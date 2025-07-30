#pragma once
#include "Asset/inspectable_asset.h"
#include "Rendering/buffers.h"

namespace engine
{
class Texture2DImporter;
}

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

class Texture2D final : public engine::InspectableAsset, public IBuffer
{
    friend class engine::Texture2DImporter;

    std::vector<DirectX::PackedVector::XMCOLOR> tex_data;
    UINT width = 0;
    UINT height = 0;
    UINT16 mip_level;
    DXGI_FORMAT format;

    ComPtr<ID3D12Resource> m_pResource;
    bool m_IsValid = false;

public:
    void OnInspectorGui() override;
    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    bool CanUpdate() override;
    bool IsValid() override;

    std::vector<DirectX::PackedVector::XMCOLOR> GetTexData()
    {
        return tex_data;
    }

    void SetTexData(const std::vector<DirectX::PackedVector::XMCOLOR> &resource)
    {
        tex_data = resource;
    }

    UINT Width()
    {
        return width;
    }

    UINT Height()
    {
        return height;
    }

    UINT16 MipLevel()
    {
        return mip_level;
    }

    DXGI_FORMAT Format()
    {
        return format;
    }

    ID3D12Resource *Resource();
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this), tex_data, width, height, format, mip_level);
    }
};