#pragma once
#include "editor_window.h"

namespace editor
{
class ImGuiDemoWindow : public EditorWindow
{
public:
    ImGuiDemoWindow();
    std::string Name() override;
    void OnEditorGui() override;
};
}