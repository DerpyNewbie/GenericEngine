#include "pch.h"
#include "asset_browser.h"

#include "default_editor_menus.h"
#include "gui.h"
#include "Asset/asset_database.h"
#include "logger.h"

namespace editor
{
namespace
{
bool DrawAssetHierarchyPopup(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy)
{
    if (!ImGui::BeginPopupContextItem("##ASSET_BROWSER_FILE_CONTEXT"))
    {
        return false;
    }

    if (asset_hierarchy->asset == nullptr)
    {
        ImGui::Text("There is nothing you can do here...");
        ImGui::EndPopup();
        return false;
    }

    if (ImGui::BeginMenu("Create"))
    {
        const bool created = DefaultEditorMenu::DrawAssetMenu(asset_hierarchy->asset->path_hint.parent_path());
        ImGui::EndMenu();

        // HACK: early-return on asset creation to workaround iterator issues
        if (created)
        {
            ImGui::EndPopup();
            return true;
        }
    }

    if (ImGui::MenuItem("Save"))
    {
        engine::Logger::Log<AssetBrowser>("Saving %s", asset_hierarchy->asset->path_hint.string().c_str());
        engine::AssetDatabase::WriteAsset(asset_hierarchy->asset->guid);
    }

    if (ImGui::MenuItem("Reload"))
    {
        engine::Logger::Log<AssetBrowser>("Reloading %s", asset_hierarchy->asset->path_hint.string().c_str());
        const auto descriptor = engine::AssetDatabase::GetAssetDescriptor(asset_hierarchy->asset->guid);
        descriptor->Reload();
    }

    if (ImGui::MenuItem("Reimport"))
    {
        engine::Logger::Log<AssetBrowser>("Reimporting %s", asset_hierarchy->asset->path_hint.string().c_str());
        engine::AssetDatabase::Import(asset_hierarchy->asset->path_hint);
    }

    if (ImGui::MenuItem("Delete"))
    {
        engine::Logger::Log<AssetBrowser>("Deleting %s", asset_hierarchy->asset->path_hint.string().c_str());
        engine::AssetDatabase::DeleteAsset(asset_hierarchy->asset->path_hint);
    }

    ImGui::EndPopup();
    return false;
}

bool DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGui::PushID(asset_hierarchy.get());

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    flags |= ImGuiTreeNodeFlags_NavLeftJumpsBackHere;
    flags |= ImGuiTreeNodeFlags_SpanFullWidth;

    if (!asset_hierarchy->IsDirectory())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (Editor::Instance()->SelectedObject() == asset_hierarchy)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    const bool open = ImGui::TreeNodeEx("", flags, "%s", asset_hierarchy->Name().c_str());
    if (asset_hierarchy->IsFile() && ImGui::BeginDragDropSource())
    {
        const auto guid_str = asset_hierarchy->asset->guid.str();
        ImGui::SetDragDropPayload(engine::Gui::DragDropTarget::kObjectGuid, guid_str.c_str(), guid_str.size() + 1);
        ImGui::Text("Dragging %s", asset_hierarchy->asset->path_hint.string().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        if (asset_hierarchy->asset != nullptr)
            engine::AssetDatabase::GetAsset(asset_hierarchy->asset->path_hint);
        Editor::Instance()->SetSelectedObject(asset_hierarchy);

        auto path = engine::AssetDatabase::GetProjectDirectory();
        if (asset_hierarchy->asset != nullptr)
            path = asset_hierarchy->asset->path_hint;
        if (!asset_hierarchy->IsDirectory())
            path = path.parent_path();

        Editor::Instance()->SetSelectedDirectory(path);
    }

    if (DrawAssetHierarchyPopup(asset_hierarchy))
    {
        if (open)
            ImGui::TreePop();
        ImGui::PopID();
        return true;
    }

    if (asset_hierarchy->IsDirectory() && open)
    {
        for (auto &child : asset_hierarchy->children)
        {
            // HACK: early-return when itr has changed to prevent invalidated itr issues
            if (DrawAssetHierarchy(child))
            {
                if (open)
                    ImGui::TreePop();
                ImGui::PopID();
                return true;
            }
        }
    }

    if (open)
        ImGui::TreePop();
    ImGui::PopID();
    return false;
}
}

std::string AssetBrowser::Name()
{
    return "Asset Browser";
}

void AssetBrowser::OnEditorGui()
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Reimport All"))
            engine::AssetDatabase::ImportAll();
        ImGui::EndPopup();
    }

    const auto asset_hierarchy = engine::AssetDatabase::GetRootAssetHierarchy();

    if (ImGui::BeginTable("##bg", 1, ImGuiTableFlags_RowBg))
    {
        DrawAssetHierarchy(asset_hierarchy);
        ImGui::EndTable();
    }
}
}