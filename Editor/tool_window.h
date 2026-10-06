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
    void OnEditorGui() override;
};
}