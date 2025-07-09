#include "pch.h"

#include "hierarchy.h"

#include "editor_prefs.h"
#include "gui.h"
#include "DxLib/dxlib_helper.h"
#include "DxLib/dxlib_converter.h"
#include "scene.h"
#include "scene_manager.h"
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
    auto draw = ImGui::CollapsingHeader(scene->Name().c_str());
    if (ImGui::IsItemClicked())
    {
        Editor::Instance()->SetSelectedObject(scene);
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Unload"))
            engine::SceneManager::DestroyScene(scene->Name());
        ImGui::EndPopup();
    }

    if (draw)
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
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(engine::Gui::DragDropTarget::kObjectGuid,
                                                                       ImGuiDragDropFlags_AcceptBeforeDelivery))
        {
            const auto payload_object = engine::Gui::GetObjectDragDropTarget(payload);
            const auto payload_go = engine::Gui::MakeCompatible<engine::GameObject>(payload_object);
            if (payload_go != nullptr)
            {
                const bool can_be_child = !game_object->Transform()->IsChildOf(payload_go->Transform());

                ImGui::SetTooltip(can_be_child
                                      ? "Make '%s' child of '%s'"
                                      : "Cannot make '%s' child of '%s' as its parent of child",
                                  payload_go->Name().c_str(),
                                  game_object->Name().c_str());

                if (can_be_child && payload->IsDelivery())
                {
                    engine::Logger::Log<Hierarchy>("appending %s to %s as child",
                                                   game_object->Path().c_str(), payload_go->Path().c_str());

                    payload_go->Transform()->SetParent(game_object->Transform());
                }
            }
            else
            {
                ImGui::SetTooltip("Cannot drag and drop object %s as this is not GameObject",
                                  payload_object->Name().c_str());
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
        Editor::Instance()->SetSelectedObject(game_object);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        engine::Gui::SetObjectDragDropTarget(game_object);
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