#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// ObjectのListをGuiに表示する為のクラス
/// </summary>
class ObjectListWindow : public EditorWindow
{
public:
    std::string Name() override;
    /// <summary>
    /// ObjectのListをGuiに表示します。
    /// </summary>
    void OnEditorGui() override;
};
}