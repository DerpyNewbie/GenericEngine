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
    /// <summary>
    /// AssetHierarchyのContextMenu(Create / Save / Reimport / Delete)の描画、押された際の実行を行います。
    /// </summary>
    /// <param name="object">対象のAssetHierarchy</param>
    bool OnContextMenu(std::shared_ptr<AssetHierarchy> object) override;
};
}