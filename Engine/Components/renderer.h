#pragma once
#include "component.h"
#include "event_receivers.h"
#include "transform.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
class Renderer : public Component, public IRenderReceiver
{
    friend class RenderPipeline;

protected:
    bool m_is_visible_ = false;

    virtual void UpdateBuffer();
    virtual void DepthRender();
    void SetVisible(bool visible);

public:
    DirectX::BoundingBox bounds;
    
    void OnValidate() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;
    
    virtual std::shared_ptr<Transform> BoundsOrigin() = 0;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Renderer, 1)