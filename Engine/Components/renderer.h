#pragma once
#include "component.h"
#include "transform.h"

namespace engine
{
class Renderer : public Component
{
    friend class RenderPipeline;

protected:
    bool m_is_visible_ = false;
    int m_render_queue_ = 5000;

    virtual void UpdateBuffer();
    virtual void Render() = 0;
    virtual void DepthRender();
    void SetVisible(bool visible);

public:
    DirectX::BoundingBox bounds;

    void OnInspectorGui() override;
    void OnValidate() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    virtual std::shared_ptr<Transform> BoundsOrigin() = 0;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_render_queue_));
    }
};
}