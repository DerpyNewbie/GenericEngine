#include "pch.h"

#include "cinema_brain_component.h"

#include "engine_time.h"
#include "gui.h"
#include "Animation/animation_component.h"

namespace engine
{
void CinemaBrainComponent::DoBlending(const float delta_time)
{
    const auto target = m_target_camera_.CastedLock();
    if (target == nullptr || !m_is_blending_)
    {
        return;
    }
    m_blend_.time += delta_time;
    if (m_blend_.time > m_blend_.duration)
    {
        m_is_blending_ = false;
        return;
    }

    float t = m_blend_.time / m_blend_.duration;

    m_blend_.from->ApplyTransform();
    m_blend_.to->ApplyTransform();

    TRS from_trs;
    TRS to_trs;
    m_blend_.from->GameObject()->Transform()->WorldMatrix().Decompose(from_trs.scale, from_trs.rotation, from_trs.translate);
    m_blend_.to->GameObject()->Transform()->WorldMatrix().Decompose(to_trs.scale, to_trs.rotation, to_trs.translate);

    auto result_trs = TRS::BlendTRS(from_trs, to_trs, t);

    auto transform = m_target_camera_.CastedLock()->GameObject()->Transform();
    transform->SetPosition(result_trs.translate);
    transform->SetRotation(result_trs.rotation);
    transform->SetLocalScale(result_trs.scale);
}

void CinemaBrainComponent::OnInspectorGui()
{
    Gui::PropertyField("Target Camera", m_target_camera_);
}

void CinemaBrainComponent::OnUpdate()
{
    DoBlending(Time::GetDeltaTime());
}

void CinemaBrainComponent::Blend(const std::shared_ptr<CinemaCameraComponent> &from, const std::shared_ptr<CinemaCameraComponent> &to, float duration, float time)
{
    m_blend_.from = from;
    m_blend_.to = to;
    m_blend_.duration = duration;
    m_blend_.time = time;
    m_is_blending_ = true;
}
}

CEREAL_REGISTER_TYPE(engine::CinemaBrainComponent)