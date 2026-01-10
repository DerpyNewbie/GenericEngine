#include "pch.h"
#include "add_component_menu.h"

#include "component_factory.h"

namespace editor
{
using namespace engine;

void AddComponentMenu::Draw(const std::shared_ptr<GameObject> &go)
{
    if (go == nullptr)
    {
        ImGui::BeginDisabled();
    }

    const auto component_names = IComponentFactory::GetNames();
    std::vector<std::shared_ptr<IComponentFactory>> factories;
    for (auto component_name : component_names)
    {
        factories.emplace_back(IComponentFactory::Get(component_name));
    }

    std::unordered_map<std::string, std::vector<std::shared_ptr<IComponentFactory>>> category_map;
    for (const auto &factory : factories)
    {
        category_map[factory->Category()].emplace_back(factory);
    }

    for (const auto &[category, categorized_factories] : category_map)
    {
        if (category.empty() || ImGui::BeginMenu(category.c_str()))
        {
            for (const auto &factory : categorized_factories)
            {
                if (ImGui::MenuItem(factory->FriendlyName().c_str(), nullptr, false, go != nullptr))
                {
                    factory->AddComponentTo(go);
                }
            }

            if (!category.empty())
            {
                ImGui::EndMenu();
            }
        }
    }

    if (go == nullptr)
    {
        ImGui::EndDisabled();
    }
}
}