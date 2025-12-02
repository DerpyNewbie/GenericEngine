#pragma once
#include "Editor/editor.h"

#include <imgui.h>

namespace editor
{
class EditorWindow
{
protected:
    /// <summary>
    /// When true, DrawGui() will call OnEditorGui() without wrapping it in ImGui::Begin/End.
    /// The derived class is fully responsible for ImGui window management in OnEditorGui().
    /// Example: Used by ImGuiDemoWindow which calls ImGui::ShowDemoWindow().
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] virtual bool ShouldHandleWindowAutomatically() const
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