#pragma once
#include "editor.h"

namespace editor
{
/// <summary>
/// EditorMenuを作るときのBaseとなるクラス
/// </summary>
class EditorMenu
{
public:
    virtual ~EditorMenu() = default;
    Editor *editor;

    /// <summary>
    /// EditorMenuを表示する時に呼ばれる
    /// </summary>
    /// <param name="name">Menuの名前</param>
    virtual void OnEditorMenuGui(std::string name) = 0;
};
}