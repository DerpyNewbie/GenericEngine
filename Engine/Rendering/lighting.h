#pragma once
#include "rendering_constants.h"
#include "Components/light.h"
#include "Rendering/CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
class Lighting
{
    friend class RenderPipeline;
    friend class Light;
    // lighting related
    std::vector<std::shared_ptr<Light>> m_lights_;
    std::vector<std::shared_ptr<Light>> m_waiting_lights_;
    std::array<Matrix, RenderingConstants::kMaxShadowMapCount> m_light_view_proj_matrices_;
    std::shared_ptr<StructuredBuffer> m_light_view_proj_matrices_buffer_;
    std::shared_ptr<DescriptorHandle> m_light_view_proj_handle_;

    // depth textures related
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;
    std::vector<std::shared_ptr<DepthTexture>> m_shadow_maps_;
    std::shared_ptr<Texture2DArray> m_depth_textures_;
    std::shared_ptr<DescriptorHandle> m_shadow_map_handle_;
    std::array<std::shared_ptr<ConstantBuffer>, RenderingConstants::kMaxShadowMapCount>
    m_current_shadow_map_index_buffer_;
    std::set<int> m_free_depth_texture_handles_;
    std::shared_ptr<ConstantBuffer> m_cascade_slices_buffer_;

    void CreateShadowMapResource();

public:
    static Lighting *Instance();

    void SetCurrentShadowMapIndex(int shadow_map_index); // Will be removed after Instance ID has been implemented
    void UpdateLightsViewProjMatrixBuffer();
    void SetCascadeSlicesBuffer();
    void SetLightsViewProjMatrix() const;
    void SetShadowMap();

    void TryApplyShadow(const std::shared_ptr<Light> &light);
    void RemoveShadow(const std::shared_ptr<Light> &light);
    void AddLight(std::shared_ptr<Light> light);
    void RemoveLight(const std::shared_ptr<Light> &light);

    static void SetCascadeSlices(
        std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_slices);
};
}