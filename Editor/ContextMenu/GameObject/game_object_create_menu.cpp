#include "pch.h"
#include "game_object_create_menu.h"

#include "object_util.h"
#include "scene_manager.h"

namespace editor
{
using namespace engine;

void GameObjectCreateMenu::Draw(const std::shared_ptr<GameObject> &go)
{
    if (ImGui::MenuItem("Empty GameObject"))
    {
        const auto empty_go = GameObject::Instantiate<GameObject>("Empty GameObject");
        empty_go->SetName(ObjectUtil::GetDeduplicatedName(empty_go));

        if (go != nullptr)
        {
            empty_go->Transform()->SetParent(go->Transform());
        }
    }
}
}