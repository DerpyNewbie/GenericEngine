#pragma once
#include "component.h"
#include "event_receivers.h"
#include "renderer.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Asset/asset_ptr.h"
#include "Rendering/render_texture.h"

namespace engine
{
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

class Camera : public Component, public IDrawCallReceiver
{
    static std::weak_ptr<Camera> m_main_camera_;
    static std::weak_ptr<Camera> m_current_camera_;

    CameraProperty m_property_;
    AssetPtr<RenderTexture> m_render_texture_;

    unsigned int m_drawcall_count_ = 0;

    void Render();
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_;

    void SetViewProjMatrix() const;

    [[nodiscard]] std::vector<std::shared_ptr<Renderer>> FilterVisibleObjects(
        const std::vector<std::weak_ptr<Renderer>> &renderers) const;

public:
    void OnAwake() override;
    void OnConstructed() override;
    void OnInspectorGui() override;
    int Order() override;
    void OnDraw() override;
    void OnEnabled() override;
    void OnDisabled() override;

    static std::shared_ptr<Camera> Main();
    static std::shared_ptr<Camera> Current();

    [[nodiscard]] Matrix GetViewMatrix() const;
    [[nodiscard]] Matrix GetProjectionMatrix() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_property_));
    }
};
}