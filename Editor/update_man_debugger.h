#pragma once
#include "editor_window.h"

namespace editor
{
class UpdateManDebugger final : public EditorWindow
{
    std::string Name() override;
    void OnEditorGui() override;
};
}