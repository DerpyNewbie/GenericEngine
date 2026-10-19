#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// ToolWindowのGuiを管理するクラス
/// </summary>
class ToolWindow : public EditorWindow
{
public:
    std::string Name() override;
    /// <summary>
    /// ToolWindowのGuiを表示します。
    /// </summary>
    void OnEditorGui() override;
};
}