#pragma once
#include "../context_menu.h"
#include "game_object.h"

namespace editor
{
using namespace engine;
/// <summary>
/// GameObjectのContextMenuを管理するクラス
/// </summary>
class GameObjectContextMenu : public ContextMenu<GameObject>
{
    /// <summary>
    /// GameObjectのContextMenu(Create / Add Component / Duplicate / Delete)の描画、押された際の実行を行います。
    /// </summary>
    /// <param name="object">対象のGameObject。</param>
    /// <returns>常に false</returns>
    bool OnContextMenu(std::shared_ptr<GameObject> object) override;
};
}