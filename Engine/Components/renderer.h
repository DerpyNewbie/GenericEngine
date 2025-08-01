#pragma once
#include "component.h"
#include "transform.h"

namespace engine
{
class Renderer : public Component
{
    friend class Camera;

    static std::vector<std::weak_ptr<Renderer>> renderers;

    static void Render();

protected:
    bool m_is_visible_ = false;

    virtual void OnDraw() = 0;
    void SetVisible(bool visible);

public:
    DirectX::BoundingBox bounds;

    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    virtual std::weak_ptr<Transform> BoundsOrigin() = 0;
};
}