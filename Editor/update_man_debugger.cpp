#include "pch.h"

#include "update_man_debugger.h"

#include "update_manager.h"

namespace editor
{
void UpdateManDebugger::OnConstructed()
{
    IEditorWindow::OnConstructed();
    SetName("Update Manager Debugger");
}
void UpdateManDebugger::OnEditorGui()
{
    if (ImGui::CollapsingHeader(
        ("Update Receivers: " + std::to_string(engine::UpdateManager::UpdateCount())).c_str()))
    {
        const auto &update_receivers = engine::UpdateManager::GetUpdateReceivers();
        for (size_t i = 0; i < update_receivers.size(); ++i)
        {
            const auto &receiver = update_receivers.at(i);
            ImGui::BulletText("%llu: %d, %s", i, receiver->Order(), typeid(*receiver).name());
        }
    }

    if (ImGui::CollapsingHeader(
        ("Fixed Update Receivers: " + std::to_string(engine::UpdateManager::FixedUpdateCount())).c_str()))
    {
        const auto &fixed_update_receivers = engine::UpdateManager::GetFixedUpdateReceivers();
        for (size_t i = 0; i < fixed_update_receivers.size(); ++i)
        {
            const auto &receiver = fixed_update_receivers.at(i);
            ImGui::BulletText("%llu: %d, %s", i, receiver->Order(), typeid(*receiver).name());
        }
    }

    if (ImGui::CollapsingHeader(
        ("Draw Call Receivers: " + std::to_string(engine::UpdateManager::DrawCallCount())).c_str()))
    {
        const auto &draw_call_receivers = engine::UpdateManager::GetDrawCallReceivers();
        for (size_t i = 0; i < draw_call_receivers.size(); ++i)
        {
            const auto &receiver = draw_call_receivers.at(i);
            ImGui::BulletText("%llu: %d, %s", i, receiver->Order(), typeid(*receiver).name());
        }
    }
}
}