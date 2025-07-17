#include "pch.h"

#include "box_collider.h"
#include "gui.h"

namespace engine
{
void BoxCollider::OnInspectorGui()
{
    if (Gui::Vector3Field("Extents", m_extents_))
    {
        UpdateShape();
    }

    Collider::OnInspectorGui();
}

void BoxCollider::UpdateShape()
{
    m_extents_ = Vector3{max(m_extents_.x, Mathf::kEpsilon),
                         max(m_extents_.y, Mathf::kEpsilon),
                         max(m_extents_.z, Mathf::kEpsilon)};

    m_box_shape_.setLocalScaling({m_extents_.x, m_extents_.y, m_extents_.z});
}

btCollisionShape *BoxCollider::GetShape()
{
    return &m_box_shape_;
}
}

CEREAL_REGISTER_TYPE(engine::BoxCollider)