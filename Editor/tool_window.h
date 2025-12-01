#pragma once
#include "editor_window.h"

namespace editor
{
class ToolWindow : public EditorWindow
{
public:
    std::string Name() override;
    void OnEditorGui() override;
};
}