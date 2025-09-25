#pragma once
#include "renderer.h"
#include "Rendering/depth_texture.h"
#include "Rendering/render_texture.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
class RenderPipeline;
enum class kViewMode : unsigned char
{
    kPerspective,
    kOrthographic
};

struct CameraProperty : Inspectable
{
    static constexpr float kMinFieldOfView = 1.0f;
    static constexpr float kMaxFieldOfView = 179.0f;
    static constexpr float kMinClippingPlane = 0.01f;
    static constexpr float kMaxClippingPlane = 10000.0f;

    kViewMode view_mode = kViewMode::kPerspective;
    float field_of_view = 70.0f;
    float near_plane = 0.1f;
    float far_plane = 1000.0f;
    float ortho_size = 50.0f;
    float aspect_ratio = 16.0f / 9.0f;
    Color background_color = Color(0x1A1A1AFF);

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(view_mode),
           CEREAL_NVP(field_of_view),
           CEREAL_NVP(near_plane),
           CEREAL_NVP(far_plane),
           CEREAL_NVP(ortho_size),
           CEREAL_NVP(aspect_ratio),
           CEREAL_NVP(background_color));
    }
};

class CameraComponent : public Component
{
    friend RenderPipeline;
    static std::weak_ptr<CameraComponent> m_main_camera_;
    static std::weak_ptr<CameraComponent> m_current_camera_;

    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_;

    std::shared_ptr<DepthTexture> m_depth_texture_;
    AssetPtr<RenderTexture> m_render_texture_;
    UINT m_drawcall_count_;

    void SetViewProjMatrix() const;

public:
    void OnAwake() override;
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;

    static void SetMainCamera(const std::weak_ptr<CameraComponent> &camera);
    static void SetCurrentCamera(const std::weak_ptr<CameraComponent> &camera);

    CameraProperty m_property_;

    static std::shared_ptr<CameraComponent> Main();
    static std::shared_ptr<CameraComponent> Current();

    std::vector<std::shared_ptr<Renderer>> FilterVisibleObjects(
        const std::vector<std::shared_ptr<Renderer>> &renderers) const;

    Matrix ViewMatrix() const;
    Matrix ProjectionMatrix() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_property_));
    }
};
}