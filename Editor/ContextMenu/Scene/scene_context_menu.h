#pragma once
#include "scene.h"
#include "Editor/ContextMenu/context_menu.h"

namespace editor
{
class SceneContextMenu : ContextMenu<Scene>
{
    bool OnContextMenu(std::shared_ptr<Scene> object) override;
};
}

REGISTER_CONTEXT_MENU(editor::SceneContextMenu)