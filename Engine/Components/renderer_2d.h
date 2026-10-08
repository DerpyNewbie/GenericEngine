#pragma once
#include "Math/rect.h"
#include "component.h"
#include "canvas.h"

namespace engine
{
/// <summary>
/// Canvas上に描画する2D用のRendererの基底クラスです。
/// </summary>
class Renderer2D : public Component
{
    AssetPtr<Canvas> m_canvas_;

public:
    /// <summary>
    /// GameObjectがRectTransformを持っていなければ追加します。
    /// </summary>
    void OnAwake() override;
    /// <summary>
    /// 親からCanvasを探し、見つかった場合は自身を描画対象として登録します。
    /// </summary>
    void OnEnabled() override;
    void OnDisabled() override;

    /// <summary>
    /// 所属するCanvasを設定します。
    /// </summary>
    /// <param name="canvas">所属するCanvas</param>
    void SetCanvas(const std::shared_ptr<Canvas> &canvas);
    /// <summary>
    /// RectTransformから計算した位置とサイズを、Canvasのサイズで割った値として取得します。
    /// </summary>
    /// <returns>Canvasがない場合は位置とサイズが 0 のRect</returns>
    Rect NormalizedRect() const;

    /// <summary>
    /// 描画を行います。Canvasから呼ばれます。
    /// </summary>
    virtual void Render() = 0;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_canvas_)
            );
    }
};
}

CEREAL_CLASS_VERSION(engine::Renderer2D, 1)