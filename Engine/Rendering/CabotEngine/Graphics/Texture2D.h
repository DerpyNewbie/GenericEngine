#pragma once
#include "Asset/inspectable_asset.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class Texture2DImporter;
}

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

class Texture2D : public engine::InspectableAsset, public engine::ShaderResource
{
    friend class engine::Texture2DImporter;

protected:
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

    ID3D12Resource *Resource() override;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    std::vector<DirectX::PackedVector::XMCOLOR> GetTexData()
    {
        return tex_data;
    }

    void SetTexData(const std::vector<DirectX::PackedVector::XMCOLOR> &resource)
    {
        tex_data = resource;
    }

    UINT Width() const
    {
        return width;
    }

    UINT Height() const
    {
        return height;
    }

    UINT16 MipLevel() const
    {
        return mip_level;
    }

    DXGI_FORMAT Format() const
    {
        return format;
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Object>(this), tex_data, width, height, format, mip_level);
    }
};