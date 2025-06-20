#include "pch.h"

#include "hierarchy.h"

#include "editor_prefs.h"
#include "DxLib/dxlib_helper.h"
#include "DxLib/dxlib_converter.h"
#include "scene.h"
#include "scene_manager.h"
#include "serializer.h"
#include "str_util.h"

namespace editor
{
Hierarchy::Hierarchy()
{
    window_flags = ImGuiWindowFlags_MenuBar;
}
std::string Hierarchy::Name()
{
    return "Hierarchy";
}
void Hierarchy::OnEditorGui()
{
    Editor::Instance()->DrawEditorMenuBar();
    for (const auto &current_scenes = engine::SceneManager::GetCurrentScenes();
         const auto &scene : current_scenes)
    {
        ImGui::PushID(scene.get());
        DrawScene(scene);
        ImGui::PopID();
    }

    const auto locked = selected_game_object.lock();
    if (locked != nullptr)
    {
        SetFontSize(12);
        DxLibHelper::DrawObjectInfo(StringUtil::Utf8ToShiftJis(locked->Name()).c_str(),
                                    DxLibConverter::From(locked->Transform()->WorldToLocal()));
    }
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
    ImGui::PushID(game_object.get());
    const bool has_style_color = !game_object->IsActiveInHierarchy();
    if (has_style_color)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImVec4(0.3F, 0.3F, 0.3F, 1.0F)));
    }

    const bool draw_tree = DrawObject(game_object);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAME_OBJECT",
                                                                       ImGuiDragDropFlags_AcceptBeforeDelivery))
        {
            const auto payload_go = static_cast<std::shared_ptr<engine::GameObject> *>(payload->Data);
            if (*payload_go != nullptr)
            {
                const bool can_be_child = !game_object->Transform()->IsChildOf((*payload_go)->Transform());

                ImGui::SetTooltip(can_be_child
                                      ? "Make '%s' child of '%s'"
                                      : "Cannot make '%s' child of '%s' as its parent of child",
                                  payload_go->get()->Name().c_str(),
                                  game_object->Name().c_str());

                if (can_be_child && payload->IsDelivery())
                {
                    engine::Logger::Log<Hierarchy>("appending %s to %s as child",
                                                   game_object->Path().c_str(), (*payload_go)->Path().c_str());

                    (*payload_go)->Transform()->SetParent(game_object->Transform());
                }
            }
        }

        ImGui::EndDragDropTarget();
    }

    if (draw_tree)
    {
        const auto transform = game_object->Transform();
        // Child count cannot be stored as it can be changed during DrawObjectRecursive
        for (int i = 0; i < transform->ChildCount(); ++i)
        {
            const auto child_go = transform->GetChild(i)->GameObject();
            DrawObjectRecursive(child_go);
        }
        ImGui::TreePop();
    }

    if (has_style_color)
    {
        ImGui::PopStyleColor();
    }

    ImGui::PopID();
}
bool Hierarchy::DrawObject(const std::shared_ptr<engine::GameObject> &game_object)
{
    bool is_tree_expanded = false;
    if (game_object->Transform()->ChildCount() > 0)
    {
        is_tree_expanded = ImGui::TreeNode("##CHILD_TREE");
    }
    else
    {
        ImGui::Dummy(ImVec2{20, 2});
    }

    ImGui::SameLine();

    const auto locked = selected_game_object.lock();
    if (ImGui::Selectable(game_object->Name().c_str(), game_object == locked))
    {
        selected_game_object = game_object;
        Editor::Instance()->SetSelectedObject(locked);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload(
            "HIERARCHY_GAME_OBJECT", &game_object,
            sizeof(std::shared_ptr<engine::GameObject> *));

        ImGui::Text("Dragging %s", game_object->Path().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text("Game Object Context Menu for %s", game_object->Path().c_str());
        if (ImGui::MenuItem("Delete"))
        {
            engine::Object::Destroy(game_object);
        }

        ImGui::EndPopup();
    }

    return is_tree_expanded;
}

}