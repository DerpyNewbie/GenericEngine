#include "pch.h"

#include "cinema_brain_component.h"

#include "engine_time.h"
#include "gui.h"
#include "Math/trs.h"

namespace engine
{
void CinemaBrainComponent::DoBlending(const float delta_time)
{
    const auto target = m_target_camera_.CastedLock();
    if (target == nullptr || !m_is_blending_)
    {
        return;
    }

    const auto transform = target->GameObject()->Transform();
    if (transform == nullptr)
    {
        return;
    }

    m_blend_.time += delta_time;
    if (m_blend_.time > m_blend_.duration)
    {
        m_blend_.time = m_blend_.duration;
        m_is_blending_ = false;

        const TRS to_trs{m_blend_.to->GameObject()->Transform()->WorldMatrix()};

        transform->SetPosition(to_trs.translation);
        transform->SetRotation(to_trs.rotation);
        transform->SetLocalScale(to_trs.scale);
        return;
    }

    const float t = 1.0f - m_blend_.time / m_blend_.duration;

    m_blend_.from->ApplyTransform();
    m_blend_.to->ApplyTransform();

    const TRS from_trs{m_blend_.from->GameObject()->Transform()->WorldMatrix()};
    const TRS to_trs{m_blend_.to->GameObject()->Transform()->WorldMatrix()};
    const auto result_trs = TRS::Blend(from_trs, to_trs, t);

    transform->SetPosition(result_trs.translation);
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

bool CinemaBrainComponent::Blending()
{
    return m_is_blending_;
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