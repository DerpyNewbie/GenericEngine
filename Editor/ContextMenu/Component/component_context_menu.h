#pragma once
#include "../context_menu.h"
#include "Components/component.h"

namespace editor
{
/// <summary>
/// ComponentのContextMenuを管理するクラス
/// </summary>
class ComponentContextMenu : public ContextMenu<Component>
{
    bool OnContextMenu(std::shared_ptr<Component> component) override;
};
}