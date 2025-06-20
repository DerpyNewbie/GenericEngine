#include "pch.h"
#include "default_editor_menus.h"

#include "editor_prefs.h"
#include "scene_manager.h"
#include "serializer.h"
#include "game_object.h"
#include "scene.h"
#include "Components/camera.h"
#include "Components/controller.h"
#include "Components/cube_renderer.h"
#include "Components/frame_meta_data.h"
#include "Components/mesh_renderer.h"
#include "Components/mv1_renderer.h"
#include "Components/skinned_mesh_renderer.h"

void editor::DefaultEditorMenu::OnEditorMenuGui(const std::string name)
{
    if (name == "Default")
    {
        DrawDefaultMenu();
        return;
    }

    if (name == "Files")
    {
        DrawFilesMenu();
        return;
    }

    if (name == "Edit")
    {
        DrawEditMenu();
        return;
    }

    if (name == "Object")
    {
        DrawObjectMenu();
        return;
    }

    if (name == "Component")
    {
        DrawComponentMenu();
        return;
    }

    if (name == "Window")
    {
        DrawWindowMenu();
        return;
    }

    throw std::runtime_error("Unknown editor menu: " + name);
}
void editor::DefaultEditorMenu::DrawDefaultMenu()
{
    DrawFilesMenu();
    DrawEditMenu();
    DrawObjectMenu();
    DrawWindowMenu();
}
void editor::DefaultEditorMenu::DrawFilesMenu()
{
    if (ImGui::BeginMenu("Files"))
    {
        if (ImGui::MenuItem("Unload Scene"))
        {
            engine::SceneManager::DestroyScene("Default Scene");
        }
        if (ImGui::MenuItem("Load Scene"))
        {
            engine::Serializer serializer;
            engine::SceneManager::AddScene(serializer.Load<engine::Scene>("Default Scene.json"));
        }
        if (ImGui::MenuItem("Save Scene"))
        {
            engine::Serializer serializer;
            serializer.Save(engine::SceneManager::GetActiveScene());
        }
        ImGui::EndMenu();
    }
}
void editor::DefaultEditorMenu::DrawEditMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::BeginMenu("Prefs"))
        {
            ImGui::MenuItem("Show Grid", nullptr, &EditorPrefs::show_grid);
            ImGui::Combo("Theme", &EditorPrefs::theme, "Dark\0Light\0Classic\0\0");

            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}
void editor::DefaultEditorMenu::DrawObjectMenu()
{
    if (ImGui::BeginMenu("GameObject"))
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            engine::Object::Instantiate<engine::GameObject>("Empty GameObject");
        }
        ImGui::EndMenu();
    }
}
void editor::DefaultEditorMenu::DrawComponentMenu()
{
    if (ImGui::BeginMenu("Component"))
    {
        const auto obj = Editor::Instance()->SelectedObject();
        const auto go = std::dynamic_pointer_cast<engine::GameObject>(obj);
        if (go == nullptr)
        {
            ImGui::BeginDisabled();
        }

#define ADD_COMPONENT(type) if (ImGui::MenuItem(#type)) go->AddComponent<type>();

        ADD_COMPONENT(engine::Camera)
        ADD_COMPONENT(engine::Controller)
        ADD_COMPONENT(engine::CubeRenderer)
        ADD_COMPONENT(engine::FrameMetaData)
        ADD_COMPONENT(engine::MeshRenderer)
        ADD_COMPONENT(engine::MV1Renderer)
        ADD_COMPONENT(engine::SkinnedMeshRenderer)

#undef ADD_COMPONENT

        if (go == nullptr)
        {
            ImGui::EndDisabled();
        }

        ImGui::EndMenu();
    }
}
void editor::DefaultEditorMenu::DrawWindowMenu()
{
    if (ImGui::BeginMenu("Window"))
    {
        const auto editor = Editor::Instance();
        const auto names = editor->GetEditorWindowNames();
        for (auto &name : names)
        {
            // don't allow users to disable the hierarchy window as it'll soft-lock from the editor
            if (name == "Hierarchy")
            {
                ImGui::BeginDisabled();
                ImGui::MenuItem(name.c_str(), nullptr, &editor->GetEditorWindow(name)->is_open);
                ImGui::EndDisabled();
                continue;
            }
            ImGui::MenuItem(name.c_str(), nullptr, &editor->GetEditorWindow(name)->is_open);
        }

        ImGui::EndMenu();
    }
}