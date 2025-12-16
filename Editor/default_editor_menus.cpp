#include "pch.h"
#include "default_editor_menus.h"

#include "editor_prefs.h"
#include "scene_manager.h"
#include "serializer.h"
#include "game_object.h"
#include "gui.h"
#include "scene.h"
#include "Asset/asset_database.h"

#include <shobjidl.h>

#include "ContextMenu/context_menu.h"
#include "ContextMenu/Common/add_component_menu.h"

namespace editor
{
void DefaultEditorMenu::OnEditorMenuGui(const std::string name)
{
    if (name == "Default")
    {
        DrawDefaultMenu();
        return;
    }

    if (name == "File")
    {
        DrawFilesMenu();
        return;
    }

    if (name == "Edit")
    {
        DrawEditMenu();
        return;
    }

    if (name == "GameObject")
    {
        ContextMenuRegistry::DrawMenuInline(Gui::MakeCompatible<GameObject>(Editor::Instance()->SelectedObject()));
        return;
    }

    if (name == "Component")
    {
        AddComponentMenu::Draw(std::dynamic_pointer_cast<GameObject>(Editor::Instance()->SelectedObject()));
        return;
    }

    if (name == "Window")
    {
        DrawWindowMenu();
        return;
    }

    if (name == "Asset")
    {
        DrawAssetMenu(editor->SelectedDirectory());
        return;
    }

    throw std::runtime_error("Unknown editor menu: " + name);
}

void DefaultEditorMenu::DrawDefaultMenu()
{
    DrawFilesMenu();
    DrawEditMenu();
    DrawWindowMenu();
}

void DefaultEditorMenu::DrawFilesMenu()
{
    static std::vector<FilterSpec> scene_filter =
    {
        {L"Scene Files (*.scene)", L"*.scene"},
        {L"All Files (*.*)", L"*.*"}
    };

    if (ImGui::MenuItem("Load Scene"))
    {
        std::string file_path = AssetDatabase::GetProjectDirectory().string() + "\\";

        if (!Gui::OpenFileDialog(file_path, scene_filter))
        {
            Logger::Error<DefaultEditorMenu>("Failed to open file dialog");
            return;
        }

        std::ifstream ifs(file_path);
        std::stringstream ss;
        ss << ifs.rdbuf();
        SceneManager::DeserializeScene(ss.str());
    }

    if (ImGui::MenuItem("Save Scene"))
    {
        std::string file_path = AssetDatabase::GetProjectDirectory().string();

        auto target_scene = SceneManager::GetActiveScene();
        auto default_file_name = target_scene->Name() + ".scene";
        if (!Gui::SaveFileDialog(file_path, default_file_name, scene_filter))
        {
            Logger::Error<DefaultEditorMenu>("Failed to open save file dialog");
            return;
        }

        Serializer serializer;
        std::ofstream ofs(file_path);
        if (serializer.Save(ofs, target_scene))
        {
            Gui::OkDialog("Scene saved", "Scene saved successfully", {Gui::MbDialogIcon::kInfo});
        }
        else
        {
            Gui::OkDialog("Scene save failed", "Failed to save a scene!\nCheck logs for more details.", {Gui::MbDialogIcon::kError});
        }

    }
}

void DefaultEditorMenu::DrawEditMenu()
{
    if (ImGui::BeginMenu("Prefs"))
    {
        ImGui::MenuItem("Show Grid", nullptr, &EditorPrefs::show_grid);
        ImGui::MenuItem("Show Physics Debug", nullptr, &EditorPrefs::show_physics_debug);
        ImGui::Combo("Theme", &EditorPrefs::theme, "Dark\0Light\0Classic\0\0");

        ImGui::EndMenu();
    }
}

void DefaultEditorMenu::DrawWindowMenu()
{
    const auto editor = Editor::Instance();
    const auto names = editor->GetEditorWindowNames();
    for (auto &name : names)
    {
        ImGui::MenuItem(name.c_str(), nullptr, &editor->GetEditorWindow(name)->is_open);
    }
}

bool DefaultEditorMenu::DrawAssetMenu(const path &path)
{
    const auto editor = Editor::Instance();
    const auto menus = editor->GetCreateMenus();
    const auto enabled = !path.empty();
    for (const auto &menu : menus)
    {
        if (ImGui::MenuItem(menu.name.c_str(), nullptr, false, enabled))
        {
            const auto object = menu.factory();
            auto file_name = object->Name() + menu.extension;

            int i = 0;
            while (exists(path / file_name))
            {
                file_name = object->Name() + " " + std::to_string(++i) + menu.extension;
            }

            AssetDatabase::CreateAsset(object, path / file_name);
            return true;
        }

        if (!enabled)
        {
            ImGui::SetItemTooltip("You must select directory in AssetBrowser in order to create asset");
        }
    }

    return false;
}
}