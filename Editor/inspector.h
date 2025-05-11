#pragma once
#include "editor_window.h"

namespace engine
{
class GameObject;
}

namespace editor
{
class Inspector : public IEditorWindow
{
public:
    std::shared_ptr<engine::GameObject> *selected_game_object_ptr;
    void OnConstructed() override;
    void OnEditorGui() override;
};
}