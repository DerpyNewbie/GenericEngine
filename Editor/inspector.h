#pragma once
#include "editor_window.h"

namespace engine
{
class GameObject;
}

namespace editor
{
class Inspector final : public EditorWindow
{
public:
    std::shared_ptr<engine::GameObject> *selected_game_object_ptr;

    explicit Inspector(std::shared_ptr<engine::GameObject> *selected_obj_ptr);
    std::string Name() override;
    void OnEditorGui() override;
};
}