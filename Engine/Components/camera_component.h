#pragma once
#include "renderer.h"
#include "Rendering/depth_texture.h"
#include "Rendering/render_texture.h"

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
    Matrix ProjectionMatrix() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(view_mode),
            CEREAL_NVP(field_of_view),
            CEREAL_NVP(near_plane),
            CEREAL_NVP(far_plane),
            CEREAL_NVP(ortho_size),
            CEREAL_NVP(aspect_ratio),
            CEREAL_NVP(background_color)
        );
    }
};

class CameraComponent : public Component
{
    friend RenderPipeline;
    inline static std::weak_ptr<CameraComponent> m_main_camera_;
    inline static std::weak_ptr<CameraComponent> m_current_camera_;
    inline static std::list<std::weak_ptr<CameraComponent>> m_cameras_;

    AssetPtr<DepthTexture> m_depth_texture_;
    AssetPtr<RenderTexture> m_render_texture_;
    UINT m_drawcall_count_;

public:

    void OnAwake() override;
    void OnInspectorGui() override;
    void OnValidate() override;
    void OnEnabled() override;
    void OnDisabled() override;

    std::shared_ptr<RenderTexture> RenderTexture();

    static void SetMainCamera(const std::weak_ptr<CameraComponent> &camera);
    static void SetCurrentCamera(const std::weak_ptr<CameraComponent> &camera);

    CameraProperty m_property_;

    static std::shared_ptr<CameraComponent> Main();
    static std::shared_ptr<CameraComponent> Current();

    Matrix ViewMatrix() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_property_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::CameraProperty, 1)

CEREAL_CLASS_VERSION(engine::CameraComponent, 1)