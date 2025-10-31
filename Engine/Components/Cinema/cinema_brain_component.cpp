#include "pch.h"

#include "cinema_brain_component.h"

#include "engine_time.h"
#include "gui.h"
#include "Animation/animation_component.h"

namespace engine
{
void CinemaBrainComponent::OnInspectorGui()
{
    Gui::PropertyField("Target Camera", m_target_camera_);
}

void CinemaBrainComponent::OnUpdate()
{
    const auto target = m_target_camera_.CastedLock();
    if (target == nullptr || !m_blend_.is_blending)
    {
        return;
    }
    m_blend_.time += Time::GetDeltaTime();
    if (m_blend_.time > m_blend_.duration)
    {
        m_blend_.is_blending = false;
        return;
    }

    TRS final_trs;

    final_trs.rotation = Quaternion::Identity;

    float t = m_blend_.time / m_blend_.duration;

    m_blend_.from->ApplyTransform();
    m_blend_.to->ApplyTransform();

    Vector3 pos, sca;
    Quaternion rot;

    m_blend_.from->GameObject()->Transform()->WorldMatrix().Decompose(sca, rot, pos);
    final_trs.translate += pos * t;
    final_trs.scale += sca * t;
    final_trs.rotation = Mathf::Slerp(final_trs.rotation, rot, t);

    t = 1.f - t;
    m_blend_.to->GameObject()->Transform()->WorldMatrix().Decompose(sca, rot, pos);
    final_trs.translate += pos * t;
    final_trs.scale += sca * t;
    final_trs.rotation = Mathf::Slerp(final_trs.rotation, rot, t);

    auto transform = m_target_camera_.CastedLock()->GameObject()->Transform();
    transform->SetPosition(final_trs.translate);
    transform->SetRotation(final_trs.rotation);
    transform->SetLocalScale(final_trs.scale);
}

void CinemaBrainComponent::Blend(const std::shared_ptr<CinemaCameraComponent> &from, const std::shared_ptr<CinemaCameraComponent> &to, float duration, float time)
{
    m_blend_.from = from;
    m_blend_.to = to;
    m_blend_.duration = duration;
    m_blend_.time = time;
    m_blend_.is_blending = true;
}
}

CEREAL_REGISTER_TYPE(engine::CinemaBrainComponent)