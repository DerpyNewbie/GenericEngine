#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// Editorの設定のGuiを管理するクラス
/// </summary>
class EditorPrefs final : public EditorWindow
{
public:
    inline static bool show_editor_debug = false;
    inline static bool show_grid = true;
    inline static bool show_physics_debug = true;
    inline static int theme = 0;

    std::string Name() override;
    /// <summary>
    /// Editorの設定のGuiを表示します。
    /// </summary>
    void OnEditorGui() override;
};
}