#pragma once
#include "editor_window.h"

namespace editor
{
class UpdateManDebugger final : public IEditorWindow
{
    void OnConstructed() override;
    void OnEditorGui() override;
};
}