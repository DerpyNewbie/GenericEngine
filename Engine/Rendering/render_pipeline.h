#pragma once
#include "event.h"
#include "rendering_settings_component.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{

struct CameraProperty;
class Renderer;
class DepthTexture;

class RenderPipeline
{
    friend class Light;
    friend class Engine;
    friend class CameraComponent;


    constexpr static int kMaxLightCount = 10;
    static constexpr Vector2 kShadowMapSize = {1920, 1065};
    static constexpr UINT kMaxShadowMapCount = 10;

    //ライト関係
    std::vector<std::shared_ptr<Light>> m_lights_;
    std::array<Matrix, kMaxShadowMapCount> m_light_view_proj_matrices_;
    std::shared_ptr<StructuredBuffer> m_light_view_proj_matrices_buffer_;
    std::shared_ptr<DescriptorHandle> m_light_view_proj_handle_;

    //深度テクスチャ関係
    std::vector<std::shared_ptr<DepthTexture>> m_shadow_maps_;
    std::shared_ptr<Texture2DArray> m_depth_textures_;
    std::shared_ptr<DescriptorHandle> m_shadow_map_handle_;
    std::array<std::shared_ptr<ConstantBuffer>, kMaxShadowMapCount> m_current_shadow_map_index_buffer_;
    std::set<int> m_free_depth_texture_handles_;
    std::shared_ptr<ConstantBuffer> m_cascade_slices_buffer_;

    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::unordered_set<std::shared_ptr<CameraComponent>> m_cameras_;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_;

    void InvokeDrawCall();
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    void Render(const Matrix &view, const CameraProperty &camera_property);
    void DepthRender();

    void SetCurrentShadowMapIndex(int shadow_map_index); //この処理はInstanceIDの実装により消されます
    void UpdateLightsViewProjMatrixBuffer();
    void SetCascadeSlicesBuffer();
    void SetLightsViewProjMatrix() const;
    void SetShadowMap();

public:
    Event<> on_rendering;

    static RenderPipeline *Instance();
    static size_t GetRendererCount();

    void AddLight(std::shared_ptr<Light> light);
    void RemoveLight(const std::shared_ptr<Light> &light);
    static void AddCamera(std::shared_ptr<CameraComponent> camera);
    static void RemoveCamera(const std::shared_ptr<CameraComponent> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);

    static void SetCascadeSlices(
        std::array<float, RenderingSettingsComponent::kShadowCascadeCount> shadow_cascade_slices);
};
}