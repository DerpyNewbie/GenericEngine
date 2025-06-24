#include "pch.h"
#include "asset_browser.h"

#include "asset_database.h"
#include "engine_util.h"


std::string editor::AssetBrowser::Name()
{
    return "Asset Browser";
}
static void DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy)
{
    if (asset_hierarchy->IsFile())
    {
        if (ImGui::Selectable(asset_hierarchy->asset->path.filename().string().c_str()))
        {
            editor::Editor::Instance()->SetSelectedObject(asset_hierarchy);
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
}
void editor::AssetBrowser::OnEditorGui()
{
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Reimport All"))
            engine::AssetDatabase::ImportAll();
        ImGui::EndPopup();
        return;
    }

    const auto asset_hierarchy = engine::AssetDatabase::GetRootAssetHierarchy();
    DrawAssetHierarchy(asset_hierarchy);
}