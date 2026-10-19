#pragma once
#include "editor_window.h"
#include "inspectable.h"
#include "scene.h"

namespace engine
{
class Component;
class AssetHierarchy;
class GameObject;
}

namespace editor
{
/// <summary>
/// InspectorのGuiを管理するクラス
/// </summary>
class Inspector final : public EditorWindow
{
    bool m_locked_ = false;
    std::weak_ptr<engine::Object> m_last_seen_object_;

    /// <summary>
    /// 指定されたObjectの型にあった情報をGuiに表示します。
    /// </summary>
    /// <param name="object">Object</param>
    static void DrawObject(const std::shared_ptr<engine::Object> &object);

    /// <summary>
    /// Sceneに関する情報をGuiに表示します。
    /// </summary>
    /// <param name="scene">表示されるScene</param>
    static void DrawScene(const std::shared_ptr<engine::Scene> &scene);

    /// <summary>
    /// GameObjectに関する情報をGuiに表示します。 GameObjectのComponentも表示されます。
    /// </summary>
    /// <param name="game_object">表示されるGameObject</param>
    static void DrawGameObject(const std::shared_ptr<engine::GameObject> &game_object);

    /// <summary>
    /// Componentに関する情報を表示します。
    /// </summary>
    /// <param name="component">表示されるComponent</param>
    static void DrawComponent(const std::shared_ptr<engine::Component> &component);

    /// <summary>
    /// InspectableなObjectのOnInspectorGuiを呼び出します。
    /// </summary>
    /// <param name="inspectable">表示されるinspectable</param>
    static void DrawInspectable(const std::shared_ptr<engine::Inspectable> &inspectable);

    /// <summary>
    /// AssetのHierarchyを表示します。子がないAssetである場合は自身の情報を表示し、子があるAssetは子のAssetの情報を表示します。
    /// </summary>
    /// <param name="asset_hierarchy">表示されるAsset</param>
    /// <param name="root">最初に表示されるAssetであるかどうか</param>
    static void DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy, bool root = true);

    /// <summary>
    /// Assetに関する情報を表示します。
    /// </summary>
    /// <param name="asset_descriptor">表示されるAsset</param>
    static void DrawAssetDescriptor(const std::shared_ptr<engine::AssetDescriptor> &asset_descriptor);

public:
    std::string Name() override;
    /// <summary>
    /// 選択中のObjectの情報をGuiに表示します。
    /// </summary>
    void OnEditorGui() override;
};
}