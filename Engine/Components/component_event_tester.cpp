#include "pch.h"
#include "component_event_tester.h"

#include "gui.h"

namespace engine
{
void ComponentEventTester::Log(const std::string &message) const
{
    const auto go = GameObject();
    const auto name = go == nullptr ? "null" : go->Name();
    Logger::Log((name + "::" + message).c_str());
}
void ComponentEventTester::OnInspectorGui()
{
    Gui::PropertyField("Log Update Functions", m_log_update_functions_);
    if (m_log_update_functions_)
        Log("OnInspectorGui()");
}
void ComponentEventTester::OnUpdate()
{
    if (m_log_update_functions_)
        Log("OnUpdate()");
}
void ComponentEventTester::OnFixedUpdate()
{
    if (m_log_update_functions_)
        Log("OnFixedUpdate()");
}
void ComponentEventTester::OnRender()
{
    if (m_log_update_functions_)
        Log("OnRender()");
}
void ComponentEventTester::OnCollisionEnter(const Collision &collision)
{
    Log("OnCollisionEnter(" + collision.other->Name() + ")");
}
void ComponentEventTester::OnCollisionStay(const Collision &collision)
{
    Log("OnCollisionStay(" + collision.other->Name() + ")");
}
void ComponentEventTester::OnCollisionExit(const Collision &collision)
{
    Log("OnCollisionExit(" + collision.other->Name() + ")");
}
}

CEREAL_REGISTER_TYPE(engine::ComponentEventTester)