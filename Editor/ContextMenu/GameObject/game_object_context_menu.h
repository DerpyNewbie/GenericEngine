#pragma once
#include "../context_menu.h"
#include "game_object.h"

namespace editor
{
using namespace engine;
class GameObjectContextMenu : public ContextMenu<GameObject>
{
    bool OnContextMenu(std::shared_ptr<GameObject> object) override;
};
}