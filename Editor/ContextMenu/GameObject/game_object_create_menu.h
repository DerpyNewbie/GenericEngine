#pragma once
#include "game_object.h"

namespace editor
{
using namespace engine;
/// <summary>
/// GameObjectの作成のContextMenuを管理するクラス
/// </summary>
class GameObjectCreateMenu
{
public:
    /// <summary>
    /// 指定されたGameObjectの子Objectの作成のContextMenuの描画、押された際の実行を行います。
    /// </summary>
    /// <param name="go">親となるGameObject。 nullになれる。</param>
    static void Draw(const std::shared_ptr<GameObject> &go);
};
}