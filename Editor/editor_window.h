#pragma once
#include "Editor/editor.h"

#include <imgui.h>

namespace editor
{
/// <summary>
/// Editorに表示するWindowの基底クラスです。
/// </summary>
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

    /// <summary>
    /// このWindowの名前を返します。Windowのタイトルとして使われます。
    /// </summary>
    /// <returns>Windowの名前。overrideされていない場合はクラス名</returns>
    virtual std::string Name();
    /// <summary>
    /// WindowのGuiを表示する時に呼ばれる
    /// </summary>
    virtual void OnEditorGui() = 0;

    /// <summary>
    /// Windowが開かれている場合、WindowのGuiを表示します。
    /// </summary>
    void DrawGui();
};
}