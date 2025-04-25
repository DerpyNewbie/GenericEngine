#pragma once
#include "../Engine/object.h"

namespace editor
{
class IEditorWindow : public engine::Object
{
public:
    bool is_open = true;
    virtual void OnEditorGui() = 0;
    void DrawGui();
};
}