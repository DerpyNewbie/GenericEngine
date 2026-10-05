#pragma once
#include "Asset/asset_hierarchy.h"
#include "Editor/ContextMenu/context_menu.h"

namespace editor
{
/// <summary>
/// AssetHierarchyのContextMenuを管理するクラス
/// </summary>
class AssetHierarchyContextMenu : public ContextMenu<AssetHierarchy>
{
public:
    bool OnContextMenu(std::shared_ptr<AssetHierarchy> object) override;
};
}