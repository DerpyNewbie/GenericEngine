#pragma once
#include "Components/renderer.h"
#include "Components/camera_component.h"

namespace engine
{
class Renderer2D;

/// <summary>
/// Renderer2Dの描画順を決めるための比較関数です。
/// </summary>
struct RendererComparator
{
    /// <summary>
    /// CanvasからたどったHierarchy上の並び順(SiblingIndex)で2つのRenderer2Dを比較します。
    /// </summary>
    /// <returns>a が b より先に描画される場合 true</returns>
    bool operator()(const std::shared_ptr<Renderer2D> &a, const std::shared_ptr<Renderer2D> &b) const;
};
/// <summary>
/// 子のRenderer2Dをまとめて描画する、2D描画用のCanvasです。
/// </summary>
class Canvas : public Renderer
{
    Vector2 m_canvas_size_;
    AssetPtr<CameraComponent> m_target_camera_;
    std::set<std::shared_ptr<Renderer2D>, RendererComparator> m_child_renderers_;

public:
    void OnInspectorGui() override;
    /// <summary>
    /// CanvasのサイズをWindowのサイズに合わせ、Target Cameraが設定されていなければMain Cameraを設定します。
    /// </summary>
    void OnAwake() override;
    /// <summary>
    /// 子が持つすべてのRenderer2Dを、このCanvasに登録します。
    /// </summary>
    void OnStart() override;
    /// <summary>
    /// 現在描画中のCameraがTarget Cameraである場合に、登録されているRenderer2Dを順に描画します。
    /// </summary>
    void Render() override;
    /// <summary>
    /// Canvasのサイズを取得します。
    /// </summary>
    [[nodiscard]] Vector2 CanvasSize() const;
    /// <summary>
    /// boundsの基準となるMatrixとして、Main CameraのWorldMatrixを返します。
    /// </summary>
    Matrix BoundsOrigin() override;

    /// <summary>
    /// Renderer2Dを描画対象として登録します。
    /// </summary>
    /// <param name="renderer">登録するRenderer2D</param>
    void AddRenderer(const std::shared_ptr<Renderer2D> &renderer);
    /// <summary>
    /// Renderer2Dを描画対象から取り除きます。
    /// </summary>
    /// <param name="renderer">取り除くRenderer2D</param>
    void RemoveRenderer(const std::shared_ptr<Renderer2D> &renderer);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_canvas_size_), CEREAL_NVP(m_target_camera_));
    }
};
}

CEREAL_CLASS_VERSION(engine::Canvas, 1)