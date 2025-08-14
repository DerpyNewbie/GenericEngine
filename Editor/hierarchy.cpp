#include "pch.h"

#include "hierarchy.h"

#include "default_editor_menus.h"
#include "editor_prefs.h"
#include "gui.h"
#include "scene.h"
#include "scene_manager.h"
#include "str_util.h"

namespace editor
{
std::string Hierarchy::Name()
{
    return "Hierarchy";
}

void Hierarchy::OnEditorGui()
{
    for (const auto &current_scenes = engine::SceneManager::GetCurrentScenes();
         const auto &scene : current_scenes)
    {
        ImGui::PushID(scene.get());
        DrawScene(scene);
        ImGui::PopID();
    }

    auto available_space = ImGui::GetContentRegionAvail();
    ImGui::PushID("HIERARCHY_EMPTY_SPACE");
    ImGui::Dummy(available_space);
    if (ImGui::BeginPopupContextItem("HIERARCHY_EMPTY_SPACE"))
    {
        DefaultEditorMenu::DrawObjectMenu();
        ImGui::EndPopup();
    }
    ImGui::PopID();
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
        if (ImGui::BeginTable("##bg", 1, ImGuiTableFlags_RowBg))
        {
            for (const auto all_root_objects = scene->RootGameObjects();
                 const auto &root_object : all_root_objects)
            {
                DrawObjectRecursive(root_object);
            }
            ImGui::EndTable();
        }
    }
}

void Hierarchy::DrawObjectRecursive(const std::shared_ptr<engine::GameObject> &game_object)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGui::PushID(game_object.get());

    const bool has_style_color = !game_object->IsActiveInHierarchy();
    if (has_style_color)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImVec4(0.3F, 0.3F, 0.3F, 1.0F)));
    }

    ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
    DrawReorderingTarget(game_object, 0);
    ImGui::PopStyleVar();

    if (DrawObject(game_object))
    {
        const auto transform = game_object->Transform();
        if (transform->ChildCount() != 0)
        {
            DrawReorderingTarget(transform->GetChild(0)->GameObject(), 0);
        }

        // Child count cannot be stored as it can be changed during DrawObjectRecursive
        for (int i = 0; i < transform->ChildCount(); ++i)
        {
            const auto child_go = transform->GetChild(i)->GameObject();
            DrawObjectRecursive(child_go);
        }
        ImGui::TreePop();
    }

    ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
    DrawReorderingTarget(game_object, 1);
    ImGui::PopStyleVar();

    if (has_style_color)
    {
        ImGui::PopStyleColor();
    }

    ImGui::PopID();
}

bool Hierarchy::DrawObject(const std::shared_ptr<engine::GameObject> &game_object)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    flags |= ImGuiTreeNodeFlags_NavLeftJumpsBackHere;
    flags |= ImGuiTreeNodeFlags_SpanFullWidth;

    if (game_object->Transform()->ChildCount() == 0)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (game_object == Editor::Instance()->SelectedObject())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
    const bool open = ImGui::TreeNodeEx("", flags, "%s", game_object->Name().c_str());
    ImGui::PopStyleVar();

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        Editor::Instance()->SetSelectedObject(game_object);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        engine::Gui::SetObjectDragDropTarget(game_object);
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginPopupContextItem())
    {
        DefaultEditorMenu::DrawObjectMenu(game_object);
        ImGui::EndPopup();
    }

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

    return open;
}

void Hierarchy::DrawReorderingTarget(const std::shared_ptr<engine::GameObject> &game_object, const int offset)
{
    constexpr float reorder_target_height = 0.0001F;

    auto reorder_target_size = ImGui::GetContentRegionAvail();
    reorder_target_size.y = reorder_target_height;
    ImGui::Dummy(reorder_target_size);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(engine::Gui::DragDropTarget::kObjectGuid,
                                                                       ImGuiDragDropFlags_AcceptBeforeDelivery))
        {
            const auto payload_object = engine::Gui::GetObjectDragDropTarget(payload);
            const auto payload_go = engine::Gui::MakeCompatible<engine::GameObject>(payload_object);
            if (payload_go != nullptr)
            {
                const bool can_be_parent = !game_object->Transform()->IsChildOf(payload_go->Transform());

                if (can_be_parent && payload->IsDelivery())
                {
                    payload_go->Transform()->SetParent(game_object->Transform()->Parent());
                    payload_go->Transform()->SetSiblingIndex(game_object->Transform()->GetSiblingIndex() + offset);
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}

}