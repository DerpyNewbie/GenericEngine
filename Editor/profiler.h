#pragma once
#include "editor_window.h"

class Profiler final : public editor::IEditorWindow
{
public:
    void OnConstructed() override;
    void OnEditorGui() override;
};