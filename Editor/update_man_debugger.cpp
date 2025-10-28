#include "pch.h"

#include "update_man_debugger.h"

#include "game_object.h"
#include "update_manager.h"

namespace editor
{
template <class T>
static void DrawBulletText(const std::vector<std::weak_ptr<T>> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        const auto &receiver = vec.at(i);
        const auto locked = receiver.lock();
        if (locked == nullptr)
        {
            ImGui::BulletText("%llu: NULL", i);
            continue;
        }

        const auto component = std::dynamic_pointer_cast<engine::Component>(locked);
        if (component != nullptr)
        {
            ImGui::BulletText("%llu: %d, %s, %s",
                              i,
                              locked->Order(),
                              typeid(*locked).name(),
                              component->GameObject()->Path().c_str());
            continue;
        }

        ImGui::BulletText("%llu: %d, %s", i, locked->Order(), typeid(*locked).name());
    }
}
std::string UpdateManDebugger::Name()
{
    return "Update Manager Debugger";
}
void UpdateManDebugger::OnEditorGui()
{
    if (ImGui::CollapsingHeader(
        ("Update Receivers: " + std::to_string(engine::UpdateManager::UpdateCount())).c_str()))
    {
        const auto &update_receivers = engine::UpdateManager::GetUpdateReceivers();
        DrawBulletText(update_receivers);
    }

    if (ImGui::CollapsingHeader(
        ("Fixed Update Receivers: " + std::to_string(engine::UpdateManager::FixedUpdateCount())).c_str()))
    {
        const auto &fixed_update_receivers = engine::UpdateManager::GetFixedUpdateReceivers();
        DrawBulletText(fixed_update_receivers);
    }
}
}