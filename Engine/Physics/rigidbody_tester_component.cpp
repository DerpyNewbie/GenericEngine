#include "pch.h"
#include "rigidbody_tester_component.h"

#include "game_object.h"
#include "gui.h"
#include "rigidbody_component.h"

namespace engine
{
void RigidbodyTesterComponent::OnFixedUpdate()
{
    auto rb = GameObject()->GetComponent<RigidbodyComponent>();
    if (rb == nullptr)
    {
        rb = GameObject()->AddComponent<RigidbodyComponent>();
    }

    switch (m_test_type_)
    {
    case kTestType::kStarting: {
        rb->SetPosition(Vector3::Zero);
        rb->SetRotation(Quaternion::Identity);
        rb->SetVelocity(Vector3::Zero);
        rb->SetAngularVelocity(Vector3::Zero);
        rb->ClearForces();
        break;
    }
    case kTestType::kForce: {
        rb->AddForce(m_force_, kForceMode::kForce);
        break;
    }
    case kTestType::kImpulse: {
        rb->AddForce(m_force_, kForceMode::kImpulse);
        break;
    }
    case kTestType::kForceAtPosition: {
        const auto world_position = Vector3::Transform(m_position_, rb->Transform()->LocalToWorld());
        rb->AddForceAtPosition(m_force_, world_position, kForceMode::kForce);
        break;
    }
    }
}
void RigidbodyTesterComponent::OnInspectorGui()
{
    int type = static_cast<int>(m_test_type_);
    ImGui::Combo("Test Type", &type, "Starting\0Force\0Impulse\0ForceAtPosition\0\0");
    m_test_type_ = static_cast<kTestType>(type);
    Gui::PropertyField<Vector3>("Force", m_force_);
    Gui::PropertyField<Vector3>("Position", m_position_);
}
}