#pragma once
#include "component.h"
#include "event_receivers.h"
#include "Asset/asset_ptr.h"
#include "Rendering/camera.h"
#include "Rendering/render_texture.h"

namespace engine
{

class CameraComponent : public Component
{

    std::shared_ptr<Camera> m_camera_;
    UINT m_drawcall_count_ = 0;

public:
    void OnAwake() override;
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}