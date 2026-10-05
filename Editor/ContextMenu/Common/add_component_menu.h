#pragma once
#include "game_object.h"

namespace editor
{
/// <summary>
/// AddComponentのContextMenuを管理するクラス
/// </summary>
class AddComponentMenu
{
public:
    /// <summary>
    /// 指定されたGameObjectに対するAddComponentのContextMenuの描画、押された際の実行を行います。
    /// </summary>
    /// <param name="go">GameObject</param>
    static void Draw(const std::shared_ptr<engine::GameObject> &go);
};
}