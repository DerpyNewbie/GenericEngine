#pragma once
#include "editor_window.h"

namespace editor
{
class ImGuiDemoWindow : public EditorWindow
{
    bool ShouldHandleWindowAutomatically() const override;
    std::string Name() override;
    void OnEditorGui() override;
};
}