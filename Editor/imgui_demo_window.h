#pragma once
#include "editor_window.h"

namespace editor
{
/// <summary>
/// ImGuiのDemoWindowを表示させる為のクラス
/// </summary>
class ImGuiDemoWindow : public EditorWindow
{
    bool ShouldHandleWindowAutomatically() const override;
    std::string Name() override;
    void OnEditorGui() override;
};
}