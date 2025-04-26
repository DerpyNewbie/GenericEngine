#pragma once
#include <memory>
#include "../Engine/game_object.h"
#include "editor_window.h"

namespace editor
{
class Hierarchy : public IEditorWindow
{
public:
    std::shared_ptr<engine::GameObject> selected_game_object;

    void OnConstructed() override;
    void OnEditorGui() override;

private:
    void DrawMenu();
    void DrawScene(const std::shared_ptr<engine::Scene> &scene);
    void DrawObjectRecursive(const std::shared_ptr<engine::GameObject> &game_object);
    bool DrawObject(const std::shared_ptr<engine::GameObject> &game_object);
};
}