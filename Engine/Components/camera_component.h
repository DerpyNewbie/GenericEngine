#pragma once
#include "component.h"
#include "event_receivers.h"
#include "Asset/asset_ptr.h"
#include "Rendering/camera.h"
#include "Rendering/render_texture.h"

namespace engine
{

class CameraComponent : public Component, public IDrawCallReceiver
{
    
    std::shared_ptr<Camera> m_camera_;
    UINT m_drawcall_count_ = 0;

public:
    void OnAwake() override;
    void OnInspectorGui() override;
    int Order() override;
    void OnDraw() override;
    void OnEnabled() override;
    void OnDisabled() override;

    std::vector<std::shared_ptr<Renderer>> FilterVisibleObjects(
        const std::vector<std::weak_ptr<Renderer>> &renderers) const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}