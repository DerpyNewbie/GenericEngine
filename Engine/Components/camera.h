#pragma once
#include "component.h"
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/render_texture.h"

namespace engine
{
class Camera : public Component, public IDrawCallReceiver
{
    enum class kViewMode : unsigned char
    {
        kPerspective,
        kOrthographic
    };

    static std::weak_ptr<Camera> m_main_camera_;

    kViewMode m_view_mode_ = kViewMode::kPerspective;
    float m_field_of_view_ = DirectX::XMConvertToRadians(70);
    float m_near_plane_ = 0.1f;
    float m_far_plane_ = 1000.0f;
    float m_ortho_size_ = 50;
    Color m_background_color_ = Color(0x1A1A1AFF);
    UINT m_drawcall_count_ = 0;

    bool BeginRender();
    void EndRender();
    void ApplyCameraSettingToDxLib() const;
    std::vector<std::shared_ptr<Renderer>> FilterVisibleObjects(const std::vector<std::weak_ptr<Renderer>> &renderers);

public:
    static constexpr float min_field_of_view = 1.0f;
    static constexpr float max_field_of_view = 179.0f;
    static constexpr float min_clipping_plane = 0.01f;
    static constexpr float max_clipping_plane = 10000.0f;

    AssetPtr<RenderTexture> render_texture;

    int Order() override;
    void OnAwake() override;
    void OnUpdate() override;
    void OnInspectorGui() override;
    void OnDraw() override;
    void OnEnabled() override;
    void OnDisabled() override;

    static std::weak_ptr<Camera> Main();

    Matrix GetViewMatrix() const;
    Matrix GetProjectionMatrix() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_view_mode_),
           CEREAL_NVP(m_field_of_view_),
           CEREAL_NVP(m_ortho_size_),
           CEREAL_NVP(m_near_plane_),
           CEREAL_NVP(m_far_plane_),
           CEREAL_NVP(m_background_color_),
           CEREAL_NVP(render_texture));
    }
};
}