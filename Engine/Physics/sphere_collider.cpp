#include "pch.h"

#include "sphere_collider.h"

#include "gui.h"

namespace engine
{
void SphereCollider::OnInspectorGui()
{
    if (Gui::FloatField("Radius", m_radius_))
    {
        UpdateShape();
    }

    Collider::OnInspectorGui();
}

void SphereCollider::UpdateShape()
{
    m_radius_ = max(m_radius_, Mathf::kEpsilon);

    m_shape_.setUnscaledRadius(m_radius_);
    MarkDirty();
}

btCollisionShape *SphereCollider::GetShape()
{
    return &m_shape_;
}

float SphereCollider::Radius() const
{
    return m_radius_;
}

void SphereCollider::SetRadius(const float radius)
{
    m_radius_ = radius;
    UpdateShape();
}
}

CEREAL_REGISTER_TYPE(engine::SphereCollider)