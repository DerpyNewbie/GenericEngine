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
    /// <summary>
    /// ComponentのContextMenu(Remove / Add Component)の描画、押された際の実行を行います。
    /// </summary>
    /// <param name="component">対象のComponent</param>
    bool OnContextMenu(std::shared_ptr<Component> component) override;
};
}