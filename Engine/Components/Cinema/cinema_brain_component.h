#pragma once
#include "cinema_camera_component.h"

namespace engine
{

struct BlendState
{
    std::shared_ptr<CinemaCameraComponent> from;
    std::shared_ptr<CinemaCameraComponent> to;
    float duration;
    float time;
};

class CinemaBrainComponent : public Component
{
    AssetPtr<CameraComponent> m_target_camera_;
    bool m_is_blending_ = false;
    BlendState m_blend_;

    void DoBlending(float delta_time);

public:
    void OnInspectorGui() override;
    void OnUpdate() override;

    bool Blending();
    void Blend(const std::shared_ptr<CinemaCameraComponent> &from, const std::shared_ptr<CinemaCameraComponent> &to, float duration, float time = 0);

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_target_camera_));
    }
};
}