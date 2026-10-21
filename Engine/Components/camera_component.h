#pragma once
#include "renderer.h"
#include "Rendering/depth_texture.h"
#include "Rendering/render_texture.h"
#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
struct CameraTag;
}

namespace engine
{
/// <summary>
/// Cameraの投影方法です。
/// </summary>
enum class kViewMode : unsigned char
{
    kPerspective,
    kOrthographic
};

/// <summary>
/// Cameraの設定(FieldOfView、Near / Far、ViewModeなど)です。
/// </summary>
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
    /// <summary>
    /// ViewModeに応じて、右手系の透視投影または平行投影のProjectionMatrixを計算します。
    /// </summary>
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

/// <summary>
/// Sceneを描画するCameraのComponentです。
/// </summary>
class CameraComponent : public Component, public IRenderReceiver
{
    inline static std::weak_ptr<CameraComponent> m_main_camera_;
    inline static std::list<std::weak_ptr<CameraComponent>> m_cameras_;

    AssetPtr<RenderTexture> m_render_texture_;
    AssetPtr<DepthTexture> m_depth_texture_;

    // CaboRenderingSystemに登録したカメラのID
    CRS::Handle<CRS::CameraTag> m_crs_camera_id_;

public:
    CameraProperty property;

    void OnInspectorGui() override;
    /// <summary>
    /// GameObjectのActive状態に合わせてOnEnabledまたはOnDisabledを呼び出します。
    /// </summary>
    void OnValidate() override;
    /// <summary>
    /// このCameraの視点と描画先をCaboRenderingSystemに登録 / 更新します。
    /// Main Cameraは画面に、それ以外はRenderTextureが設定されている場合だけそこに描きます。
    /// </summary>
    void Render() override;
    /// <summary>
    /// Cameraのリストに自身を追加し、Main CameraがなければMain Cameraになります。UpdateManagerにRenderの登録も行います。
    /// </summary>
    void OnEnabled() override;
    /// <summary>
    /// Cameraのリストから自身を取り除き、自身がMain Cameraだった場合は別のCameraをMain Cameraにします。
    /// </summary>
    void OnDisabled() override;

    /// <summary>
    /// Transformの位置と向きから、右手系のViewMatrixを計算します。
    /// </summary>
    Matrix ViewMatrix() const;

    /// <summary>
    /// 描画先のRenderTextureを取得します。設定されていない場合は新しく生成します。
    /// </summary>
    std::shared_ptr<RenderTexture> RenderTexture();
    /// <summary>
    /// 描画先に設定されているRenderTextureを取得します。設定されていない場合はnullptrを返します（生成しません）。
    /// </summary>
    std::shared_ptr<class RenderTexture> TargetTexture() const;
    /// <summary>
    /// 深度の描画先に設定されているDepthTextureを取得します。設定されていない場合はnullptrを返します。
    /// </summary>
    std::shared_ptr<DepthTexture> TargetDepthTexture() const;

    /// <summary>
    /// Main Cameraを取得します。
    /// </summary>
    /// <returns>Main Cameraがない、または破棄待ちの場合 nullptr</returns>
    static std::shared_ptr<CameraComponent> Main();
    /// <summary>
    /// Main Cameraを設定します。
    /// </summary>
    /// <param name="camera">Main CameraにするCamera</param>
    static void SetMainCamera(const std::weak_ptr<CameraComponent> &camera);
    
    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(property)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(m_render_texture_),
                CEREAL_NVP(m_depth_texture_)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::CameraProperty, 1)

CEREAL_CLASS_VERSION(engine::CameraComponent, 2)