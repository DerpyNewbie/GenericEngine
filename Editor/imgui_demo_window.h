#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// ImGuiのDemoWindowを表示させる為のクラス
/// </summary>
class ImGuiDemoWindow : public EditorWindow
{
    /// <summary>
    /// ImGui::ShowDemoWindow()が自身でWindowを管理する為、Begin/Endによる自動管理を無効にします。
    /// </summary>
    /// <returns>常に false</returns>
    bool ShouldHandleWindowAutomatically() const override;
    /// <summary>
    /// このWindowの名前を返します。Windowのタイトルとして使われます。
    /// </summary>
    /// <returns>Windowの名前</returns>
    std::string Name() override;
    /// <summary>
    /// ImGuiのDemoWindowを表示します。
    /// </summary>
    void OnEditorGui() override;
};
}