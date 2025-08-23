#include "pch.h"

#include "capsule_collider.h"
#include "gui.h"

namespace engine
{
void CapsuleCollider::OnInspectorGui()
{
    if (Gui::FloatField("Radius", m_radius_))
    {
        ApplyChanges();
    }

    if (Gui::FloatField("Height", m_height_))
    {
        ApplyChanges();
    }

    Collider::OnInspectorGui();
}

void CapsuleCollider::UpdateShape()
{
    m_radius_ = max(m_radius_, Mathf::kEpsilon);
    m_height_ = max(m_height_, Mathf::kEpsilon);
    m_capsule_shape_.setLocalScaling({m_radius_, m_height_, m_radius_});
}

btCollisionShape *CapsuleCollider::GetShape()
{
    return &m_capsule_shape_;
}

void CapsuleCollider::SetRadius(const float radius)
{
    m_radius_ = radius;
    ApplyChanges();
}

void CapsuleCollider::SetHeight(const float height)
{
    m_height_ = height;
    ApplyChanges();
}

float CapsuleCollider::Radius() const
{
    return m_radius_;
}

float CapsuleCollider::Height() const
{
    return m_height_;
}
}

CEREAL_REGISTER_TYPE(engine::CapsuleCollider)