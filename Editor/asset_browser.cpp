#include "pch.h"
#include "asset_browser.h"

#include "Asset/asset_database.h"
#include "logger.h"

std::string editor::AssetBrowser::Name()
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
            if (asset_hierarchy->asset->object == nullptr)
                engine::AssetDatabase::GetAsset(asset_hierarchy->asset->path);
            editor::Editor::Instance()->SetSelectedObject(asset_hierarchy);
        }

        if (ImGui::BeginPopupContextItem("##ASSET_BROWSER_FILE_CONTEXT"))
        {
            if (ImGui::MenuItem("Save"))
            {
                engine::Logger::Log<editor::AssetBrowser>("Saving %s", asset_hierarchy->asset->path.string().c_str());
                engine::AssetDatabase::SaveAsset(asset_hierarchy->asset->path);
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
void editor::AssetBrowser::OnEditorGui()
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