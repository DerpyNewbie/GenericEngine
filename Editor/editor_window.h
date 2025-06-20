#pragma once
#include "object.h"
#include "Editor/editor.h"

#include <imgui.h>

namespace editor
{
class EditorWindow
{
public:
    virtual ~EditorWindow() = default;

    bool is_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    Editor *editor;

    virtual std::string Name();
    virtual void OnEditorGui() = 0;

    void DrawGui();
};
}