#include "pch.h"
#include "default_editor_menus.h"

#include "component_factory.h"
#include "editor_prefs.h"
#include "scene_manager.h"
#include "serializer.h"
#include "game_object.h"
#include "gui.h"
#include "scene.h"
#include "Asset/asset_database.h"

#include <shobjidl.h>

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
        DrawObjectMenu(engine::Gui::MakeCompatible<engine::GameObject>(Editor::Instance()->SelectedObject()));
        return;
    }

    if (name == "Component")
    {
        DrawComponentMenu(std::dynamic_pointer_cast<engine::GameObject>(Editor::Instance()->SelectedObject()));
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
    DrawObjectMenu();
    DrawWindowMenu();
}

void DefaultEditorMenu::DrawFilesMenu()
{
    static std::vector<engine::FilterSpec> scene_filter =
    {
        { L"Scene Files (*.scene)", L"*.scene" },
        { L"All Files (*.*)", L"*.*" }
    };

    if (ImGui::MenuItem("Load Scene"))
    {
        std::string file_path = engine::AssetDatabase::GetProjectDirectory().string() + "\\";

        if (!engine::Gui::OpenFileDialog(file_path, scene_filter))
        {
            engine::Logger::Error<DefaultEditorMenu>("Failed to open file dialog");
            return;
        }

        engine::Serializer serializer;
        std::ifstream ifs(file_path);
        engine::SceneManager::AddScene(serializer.Load<engine::Scene>(ifs));
    }

    if (ImGui::MenuItem("Save Scene"))
    {
        std::string file_path = engine::AssetDatabase::GetProjectDirectory().string();

        auto target_scene = engine::SceneManager::GetActiveScene();
        auto default_file_name = target_scene->Name() + ".scene";
        if (!engine::Gui::SaveFileDialog(file_path, default_file_name, scene_filter))
        {
            engine::Logger::Error<DefaultEditorMenu>("Failed to open save file dialog");
            return;
        }

        engine::Serializer serializer;
        std::ofstream ofs(file_path);
        if (serializer.Save(ofs, target_scene))
        {
            engine::Gui::OkDialog("Scene saved", "Scene saved successfully", { engine::Gui::MbDialogIcon::kInfo });
        }
        else
        {
            engine::Gui::OkDialog("Scene save failed", "Failed to save a scene!\nCheck logs for more details.", { engine::Gui::MbDialogIcon::kError });
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

void DefaultEditorMenu::DrawObjectMenu(const std::shared_ptr<engine::GameObject> &go)
{
    if (ImGui::MenuItem("Create Empty"))
    {
        const auto empty_go = engine::Object::Instantiate<engine::GameObject>("Empty GameObject");
        if (go != nullptr)
        {
            empty_go->Transform()->SetParent(go->Transform());
        }
    }

    if (ImGui::MenuItem("Duplicate"))
    {
        if (const auto cloned_object = engine::Object::Instantiate(go))
        {
            engine::Logger::Log<DefaultEditorMenu>("Cloned %s", cloned_object->Name().c_str());
        }
        else
        {
            engine::Logger::Error<DefaultEditorMenu>("Failed to clone object. Check logs for more details");
        }
    }

    if (ImGui::MenuItem("Delete", nullptr, false, go != nullptr))
    {
        engine::Object::DestroyImmediate(go);
    }

    if (EditorPrefs::show_editor_debug)
    {
        if (ImGui::MenuItem("Debug: Inspect"))
        {
            std::stringstream ss;
            {
                engine::Serializer serializer;
                auto _ = serializer.Save(ss, go);
            }

            const std::string serialized_object(ss.view());
            engine::Logger::Log(serialized_object.c_str());
        }
    }
}

void DefaultEditorMenu::DrawComponentMenu(const std::shared_ptr<engine::GameObject> &go)
{
    if (go == nullptr)
    {
        ImGui::BeginDisabled();
    }

    const auto component_names = engine::IComponentFactory::GetNames();
    std::vector<std::shared_ptr<engine::IComponentFactory>> factories;
    for (auto component_name : component_names)
    {
        factories.emplace_back(engine::IComponentFactory::Get(component_name));
    }

    std::unordered_map<std::string, std::vector<std::shared_ptr<engine::IComponentFactory>>> category_map;
    for (const auto &factory : factories)
    {
        category_map[factory->Category()].emplace_back(factory);
    }

    for (const auto &[category, categorized_factories] : category_map)
    {
        if (category.empty() || ImGui::BeginMenu(category.c_str()))
        {
            for (const auto &factory : categorized_factories)
            {
                if (ImGui::MenuItem(factory->FriendlyName().c_str(), nullptr, false, go != nullptr))
                {
                    factory->AddComponentTo(go);
                }
            }

            if (!category.empty())
            {
                ImGui::EndMenu();
            }
        }
    }

    if (go == nullptr)
    {
        ImGui::EndDisabled();
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

bool DefaultEditorMenu::DrawAssetMenu(const std::filesystem::path &path)
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

            engine::AssetDatabase::CreateAsset(object, path / file_name);
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