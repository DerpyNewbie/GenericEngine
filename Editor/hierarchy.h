#pragma once
#include <memory>
#include "game_object.h"
#include "editor_window.h"

namespace editor
{
class Hierarchy final : public EditorWindow
{
public:
    std::string Name() override;
    void OnEditorGui() override;

private:
    void DrawScene(const std::shared_ptr<engine::Scene> &scene);
    void DrawObjectRecursive(const std::shared_ptr<engine::GameObject> &game_object);
    bool DrawObject(const std::shared_ptr<engine::GameObject> &game_object);
    void DrawReorderingTarget(const std::shared_ptr<engine::GameObject> &game_object, int offset);
};
}