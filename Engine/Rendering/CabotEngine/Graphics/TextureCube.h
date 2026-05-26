#pragma once
#include "Texture2D.h"
#include "Rendering/buffer_base.h"
#include "Asset/asset_ptr.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class TextureCube final : public Object, public BufferBase, public Inspectable, public ShaderResource
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource_;
    D3D12_RESOURCE_STATES m_current_state_;
    
public:
    ~TextureCube() override;
    
    void OnInspectorGui() override;
    void CreateBuffer() override;
    void UpdateBuffer(const void *data) override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_SRV;
    }
    bool IsValid() override;
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    ID3D12Resource *Resource() override;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    bool SetTextures(const std::array<AssetPtr<Texture2D>, 6> &textures);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_textures_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TextureCube, 1)