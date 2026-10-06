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
    /// <summary>
    /// SceneのContextMenu(Remove)の描画、押された際の実行を行います。
    /// </summary>
    /// <param name="object">対象のScene</param>
    bool OnContextMenu(std::shared_ptr<Scene> object) override;
};
}