#pragma once
#include "depth_texture.h"
#include "inspectable.h"
#include "render_texture.h"
#include "Asset/asset_ptr.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "Components/renderer.h"

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

class Camera : public std::enable_shared_from_this<Camera>
{
    friend CameraComponent;
    static std::weak_ptr<Camera> m_main_camera_;
    static std::weak_ptr<Camera> m_current_camera_;

    CameraProperty m_property_;

    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_;
    std::shared_ptr<Transform> m_transform_;

    std::shared_ptr<DepthTexture> m_depth_texture_;
    AssetPtr<RenderTexture> m_render_texture_;

    void SetViewProjMatrix() const;
    static void SetMainCamera(const std::shared_ptr<Camera> &camera);
    static void SetCurrentCamera(const std::shared_ptr<Camera> &camera);

public:
    CameraProperty m_property_;

    static void SetMainCamera(const std::weak_ptr<Camera> &camera);

    static std::shared_ptr<Camera> Main();
    static std::shared_ptr<Camera> Current();

    Camera();

    void Render(const std::vector<std::shared_ptr<Renderer>> &renderers);

    std::shared_ptr<Transform> GetTransform();
    Matrix ViewMatrix() const;
    Matrix ProjectionMatrix() const;
    void SetTransform(const std::shared_ptr<Transform> &transform);
};
}