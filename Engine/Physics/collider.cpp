#include "pch.h"

#include "collider.h"
#include "game_object.h"
#include "gui.h"

namespace engine
{
void Collider::AddToRigidbody()
{
    auto rb = GameObject()->GetComponentInParent<RigidbodyComponent>();
    if (rb == nullptr)
    {
        rb = GameObject()->AddComponent<RigidbodyComponent>();
    }

    m_rigidbody_ = rb;
    rb->AddCollider(shared_from_base<Collider>());
    m_is_registered_ = true;
}

void Collider::RemoveFromRigidbody()
{
    const auto locked = m_rigidbody_.lock();
    if (locked != nullptr)
    {
        locked->RemoveCollider(shared_from_base<Collider>());
    }

    m_is_registered_ = false;
}
void Collider::ApplyChanges()
{
    if (!m_is_registered_)
    {
        return;
    }

    RemoveFromRigidbody();
    UpdateShape();
    AddToRigidbody();
}

void Collider::OnInspectorGui()
{
    if (Gui::PropertyField("Offset", m_offset_))
    {
        ApplyChanges();
    }

    if (Gui::PropertyField("Is Trigger", m_is_trigger_))
    {
        ApplyChanges();
    }

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
    AddToRigidbody();
}

void Collider::OnDisabled()
{
    RemoveFromRigidbody();
}

void Collider::OnDestroy()
{
    RemoveFromRigidbody();
}

std::shared_ptr<RigidbodyComponent> Collider::Rigidbody() const
{
    return m_rigidbody_.lock();
}

bool Collider::IsTrigger() const
{
    return m_is_trigger_;
}

Vector3 Collider::Offset() const
{
    return m_offset_;
}

void Collider::SetTrigger(const bool trigger)
{
    m_is_trigger_ = trigger;
    ApplyChanges();
}

void Collider::SetOffset(const Vector3 offset)
{
    m_offset_ = offset;
    ApplyChanges();
}

}

CEREAL_REGISTER_TYPE(engine::Collider)