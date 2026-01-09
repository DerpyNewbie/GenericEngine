#pragma once
#include "editor_menu.h"

namespace editor
{
class DefaultEditorMenu final : public EditorMenu
{
public:
    void OnEditorMenuGui(std::string name) override;

    static void DrawDefaultMenu();
    static void DrawFilesMenu();
    static void DrawEditMenu();
    static void DrawWindowMenu();
    static bool DrawAssetMenu(const std::filesystem::path &path);
};
}