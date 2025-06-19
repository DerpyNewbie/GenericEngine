#pragma once
#include "editor_window.h"

namespace editor
{
class EditorPrefs : public IEditorWindow
{
public:
    static bool show_grid;
    static int theme;

    void OnConstructed() override;
    void OnEditorGui() override;
};
}