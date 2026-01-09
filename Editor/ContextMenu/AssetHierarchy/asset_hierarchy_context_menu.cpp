#include "pch.h"
#include "asset_hierarchy_context_menu.h"

#include <imgui.h>

#include "Editor/asset_browser.h"
#include "Editor/default_editor_menus.h"
#include "Engine/Asset/asset_database.h"

namespace editor
{
bool AssetHierarchyContextMenu::OnContextMenu(std::shared_ptr<AssetHierarchy> object)
{
    if (object->asset == nullptr)
    {
        ImGui::Text("There is nothing you can do here...");
        return false;
    }

    if (ImGui::BeginMenu("Create"))
    {
        const bool created = DefaultEditorMenu::DrawAssetMenu(object->asset->AssetPath().parent_path());
        ImGui::EndMenu();

        // HACK: early-return on asset creation to workaround iterator issues
        if (created)
        {
            return true;
        }
    }

    if (ImGui::MenuItem("Save"))
    {
        Logger::Log<AssetBrowser>("Saving %s", object->asset->AssetPath().string().c_str());
        AssetDatabase::WriteAsset(object->asset->Guid());
    }

    if (ImGui::MenuItem("Reimport"))
    {
        Logger::Log<AssetBrowser>("Reimporting %s", object->asset->AssetPath().string().c_str());
        AssetDatabase::Reimport(object->asset->Guid());
    }

    if (ImGui::MenuItem("Delete"))
    {
        Logger::Log<AssetBrowser>("Deleting %s", object->asset->AssetPath().string().c_str());
        AssetDatabase::DeleteAsset(object->asset->AssetPath());
    }

    return false;
}
}