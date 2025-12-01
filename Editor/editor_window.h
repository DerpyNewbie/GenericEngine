#pragma once
#include "Editor/editor.h"

#include <imgui.h>

namespace editor
{
class EditorWindow
{
protected:
    bool m_handle_window_manually_ = false;

public:
    virtual ~EditorWindow() = default;

    bool is_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    virtual std::string Name();
    virtual void OnEditorGui() = 0;

    void DrawGui();
};
}