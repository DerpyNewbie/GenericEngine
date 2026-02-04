#pragma once
#include "editor_window.h"

namespace editor
{
class ObjectListWindow : public EditorWindow
{
public:
    std::string Name() override;
    void OnEditorGui() override;
};
}