#pragma once
#include "Editor/editor.h"

#include <imgui.h>

namespace editor
{
class EditorWindow
{
protected:
    virtual bool ShouldHandleWindowAutomatically() const
    {
        return true;
    }

public:
    virtual ~EditorWindow() = default;

    bool is_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    virtual std::string Name();
    virtual void OnEditorGui() = 0;

    void DrawGui();
};
}