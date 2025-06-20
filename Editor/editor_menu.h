#pragma once
#include "editor.h"

namespace editor
{
class EditorMenu
{
public:
    virtual ~EditorMenu() = default;
    Editor *editor;

    virtual void OnEditorMenuGui(std::string name) = 0;
};
}