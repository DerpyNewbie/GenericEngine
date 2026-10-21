#pragma once
#include "component.h"
#include "event_receivers.h"
#include "transform.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"

namespace engine
{
/// <summary>
/// 描画を行うComponentの基底クラスです。
/// </summary>
class Renderer : public Component, public IRenderReceiver
{
protected:
    bool m_is_visible_ = false;

    /// <summary>
    /// 表示状態が変わった時に呼ばれます。非表示になった時に、CaboRenderingSystemから描画データを取り除くのに使います。既定では何もしません。
    /// </summary>
    /// <param name="visible">表示する場合 true</param>
    virtual void OnVisibilityChanged(bool visible);
    /// <summary>
    /// 表示状態を設定し、UpdateManagerへの登録 / 解除を行います。表示中は毎フレームRenderが呼ばれます。
    /// </summary>
    /// <param name="visible">表示する場合 true</param>
    void SetVisible(bool visible);

public:
    DirectX::BoundingBox bounds;
    std::vector<AssetPtr<Material>> shared_materials;

    /// <summary>
    /// GameObjectのActive状態に合わせて表示状態を更新します。
    /// </summary>
    void OnValidate() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    /// <summary>
    /// boundsの基準となるMatrixを取得します。
    /// </summary>
    virtual Matrix BoundsOrigin() = 0;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this)
        );

        if (version >= 2)
        {
            ar(CEREAL_NVP(shared_materials));
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Renderer, 2)