#pragma once
#include "editor_menu.h"
#include "game_object.h"

namespace editor
{
class DefaultEditorMenu final : public EditorMenu
{
public:
    void OnEditorMenuGui(std::string name) override;

    static void DrawDefaultMenu();
    static void DrawFilesMenu();
    static void DrawEditMenu();
    static void DrawObjectMenu();
    static void DrawComponentMenu(const std::shared_ptr<engine::GameObject> &go);
    static void DrawWindowMenu();
    static bool DrawAssetMenu(const std::filesystem::path &path);
};
}