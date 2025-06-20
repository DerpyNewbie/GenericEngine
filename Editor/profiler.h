#pragma once
#include "editor_window.h"

namespace editor
{
class Profiler final : public EditorWindow
{
public:
    std::string Name() override;
    void OnEditorGui() override;
};
}