#include "pch.h"

#include "collider.h"
#include "game_object.h"

namespace engine
{
void Collider::MarkDirty()
{
    m_is_dirty_ = true;
}

void Collider::OnInspectorGui()
{
    if (ImGui::CollapsingHeader("Collider Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        auto rb = Rigidbody();
        if (rb == nullptr)
        {
            ImGui::Text("not attached to rigidbody");
        }
        else
        {
            ImGui::Text("attached to rigidbody");
            ImGui::Text("path: %s", rb->GameObject()->Path().c_str());
        }

        ImGui::Text("margin: %f", GetShape()->getMargin());
        ImGui::Unindent();
    }
}
void Collider::OnEnabled()
{
    auto rb = GameObject()->GetComponentInParent<RigidbodyComponent>();
    if (rb == nullptr)
    {
        rb = GameObject()->AddComponent<RigidbodyComponent>();
    }

    m_rigidbody_ = rb;
    rb->AddCollider(shared_from_base<Collider>());
}

void Collider::OnDisabled()
{
    const auto locked = m_rigidbody_.lock();
    if (locked != nullptr)
    {
        locked->RemoveCollider(shared_from_base<Collider>());
    }
}

std::shared_ptr<RigidbodyComponent> Collider::Rigidbody() const
{
    return m_rigidbody_.lock();
}
}

CEREAL_REGISTER_TYPE(engine::Collider)