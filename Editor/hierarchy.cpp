#include "hierarchy.h"

#include "imgui.h"
#include "../Engine/scene.h"
#include "../Engine/scene_manager.h"
#include "../Engine/serializer.h"

namespace editor
{
void Hierarchy::OnConstructed()
{
    IEditorWindow::OnConstructed();
    SetName("Hierarchy");
    window_flags = ImGuiWindowFlags_MenuBar;
}
void Hierarchy::OnEditorGui()
{
    DrawMenu();
    for (const auto &current_scenes = engine::SceneManager::GetCurrentScenes();
         const auto &scene : current_scenes)
    {
        DrawScene(scene);
    }
}
void Hierarchy::DrawMenu()
{
    if (!ImGui::BeginMenuBar())
        return;

    if (ImGui::BeginMenu("Files"))
    {
        ImGui::MenuItem("Load Scene");
        if (ImGui::MenuItem("Save Scene"))
        {
            engine::Serializer serializer;
            serializer.Save(engine::SceneManager::GetActiveScene());
        }
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}
void Hierarchy::DrawScene(const std::shared_ptr<engine::Scene> &scene)
{
    if (ImGui::CollapsingHeader(scene->Name().c_str()))
    {
        for (const auto all_root_objects = scene->RootGameObjects();
             const auto &root_object : all_root_objects)
        {
            DrawObjectRecursive(root_object);
        }
    }
}
void Hierarchy::DrawObjectRecursive(const std::shared_ptr<engine::GameObject> &game_object)
{
    if (!DrawObject(game_object))
        return;
    // ImGui::Indent();
    {
        const auto transform = game_object->Transform();
        const auto child_count = transform->ChildCount();
        for (int i = 0; i < child_count; ++i)
        {
            const auto child_go = transform->GetChild(i)->GameObject();
            DrawObjectRecursive(child_go);
        }
    }
    // ImGui::Unindent();
    ImGui::TreePop();
    ImGui::PopID();
}
bool Hierarchy::DrawObject(const std::shared_ptr<engine::GameObject> &game_object)
{
    ImGui::PushID((game_object->Scene()->Name() + game_object->Path()).c_str());

    if (game_object->Transform()->ChildCount() == 0)
    {
        if (ImGui::Selectable(game_object->Name().c_str(), game_object == selected_game_object))
        {
            selected_game_object = game_object;
        }
    }
    else
    {
        bool tree_node = ImGui::TreeNode("##CHILD_TREE");
        ImGui::SameLine();
        if (ImGui::Selectable(game_object->Name().c_str(), game_object == selected_game_object))
        {
            selected_game_object = game_object;
        }

        if (tree_node)
        {
            return true;
        }
    }

    ImGui::PopID();
    return false;
}
}