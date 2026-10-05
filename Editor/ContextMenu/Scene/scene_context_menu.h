#pragma once
#include "scene.h"
#include "Editor/ContextMenu/context_menu.h"

namespace editor
{
/// <summary>
/// SceneのContextMenuを管理するクラス
/// </summary>
class SceneContextMenu : public ContextMenu<Scene>
{
    bool OnContextMenu(std::shared_ptr<Scene> object) override;
};
}