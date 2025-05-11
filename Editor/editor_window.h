#pragma once
#include "object.h"

#include <imgui.h>

namespace editor
{
class IEditorWindow : public engine::Object
{
public:
    bool is_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    virtual void OnEditorGui() = 0;
    void DrawGui();
};
}