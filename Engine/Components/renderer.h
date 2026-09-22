#pragma once
#include "component.h"
#include "event_receivers.h"
#include "transform.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/layer.h"

namespace engine
{
class Renderer : public Component, public IRenderReceiver
{
    friend class RenderPipeline;

protected:
    Layer m_rendering_layer_;
    bool m_is_visible_ = false;

    virtual void UpdateBuffer();
    virtual void DepthRender();
    void SetVisible(bool visible);

public:
    DirectX::BoundingBox bounds;
    std::vector<AssetPtr<Material>> shared_materials;

    void OnInspectorGui() override;
    void OnValidate() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    virtual Matrix BoundsOrigin() = 0;

    Layer GetRenderingLayer() const;

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
        
        if (version >= 3)
        {
            ar(CEREAL_NVP(m_rendering_layer_));
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Renderer, 3)