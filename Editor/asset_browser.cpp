#include "pch.h"
#include "asset_browser.h"

#include "gui.h"
#include "Asset/asset_database.h"
#include "logger.h"

namespace editor
{
std::string AssetBrowser::Name()
{
    return "Asset Browser";
}
static void DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy)
{
    ImGui::PushID(asset_hierarchy.get());
    if (asset_hierarchy->IsFile())
    {
        if (ImGui::Selectable(asset_hierarchy->asset->path.filename().string().c_str()))
        {
            if (asset_hierarchy->asset->managed_object == nullptr)
                engine::AssetDatabase::GetAsset(asset_hierarchy->asset->path);
            Editor::Instance()->SetSelectedObject(asset_hierarchy);
        }

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(Gui::DragDropTarget::kAsset, &asset_hierarchy,
                                      sizeof(std::shared_ptr<engine::AssetHierarchy>));
            ImGui::Text("Dragging %s", asset_hierarchy->asset->path.string().c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginPopupContextItem("##ASSET_BROWSER_FILE_CONTEXT"))
        {
            if (ImGui::MenuItem("Save"))
            {
                engine::Logger::Log<AssetBrowser>("Saving %s", asset_hierarchy->asset->path.string().c_str());
                engine::AssetDatabase::WriteAsset(asset_hierarchy->asset->guid);
            }

            ImGui::EndPopup();
        }
    }

    if (asset_hierarchy->IsDirectory())
    {
        if (ImGui::CollapsingHeader(asset_hierarchy->Name().c_str()))
        {
            ImGui::Indent();
            for (auto &child : asset_hierarchy->children)
            {
                DrawAssetHierarchy(child);
            }
            ImGui::Unindent();
        }
    }

    ImGui::PopID();
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
    DrawAssetHierarchy(asset_hierarchy);
}
}