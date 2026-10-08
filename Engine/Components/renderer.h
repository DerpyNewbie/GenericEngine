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
    friend class RenderPipeline;

protected:
    bool m_is_visible_ = false;

    /// <summary>
    /// ShadowMapへの描画の前にRenderPipelineから呼ばれ、描画に使うBufferを更新します。既定では何もしません。
    /// </summary>
    virtual void UpdateBuffer();
    /// <summary>
    /// ShadowMapへのDepthのみの描画を行います。RenderPipelineから呼ばれます。既定では何もしません。
    /// </summary>
    virtual void DepthRender();
    /// <summary>
    /// 表示状態を設定し、UpdateManagerとRenderPipelineへの登録 / 解除を行います。
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