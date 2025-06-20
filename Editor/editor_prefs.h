#pragma once
#include "editor_window.h"

namespace editor
{
class EditorPrefs final : public EditorWindow
{
public:
    static bool show_grid;
    static int theme;

    std::string Name() override;
    void OnEditorGui() override;
};
}