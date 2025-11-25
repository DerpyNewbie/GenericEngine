#pragma once
#include "editor_window.h"

namespace editor
{
class EditorPrefs final : public EditorWindow
{
public:
    static bool show_editor_debug;
    static bool show_grid;
    static bool show_physics_debug;
    static int theme;

    std::string Name() override;
    void OnEditorGui() override;
};
}