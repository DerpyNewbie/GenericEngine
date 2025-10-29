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
    bool is_blending = false;
};

class CinemaBrainComponent : public Component
{
    AssetPtr<CameraComponent> m_target_camera_;
    BlendState m_blend_;

public:
    void OnInspectorGui() override;
    void OnUpdate() override;

    void Blend(const std::shared_ptr<CinemaCameraComponent> &from, const std::shared_ptr<CinemaCameraComponent> &to, float duration, float time = 0);

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_target_camera_));
    }
};
}