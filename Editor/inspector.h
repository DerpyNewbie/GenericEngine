#pragma once
#include "editor_window.h"
#include "inspectable.h"

namespace engine
{
class AssetHierarchy;
}

namespace engine
{
class GameObject;
}

namespace editor
{
class Inspector final : public EditorWindow
{
    bool m_locked_ = false;
    std::weak_ptr<engine::Object> m_last_seen_object_;

    static void DrawObject(const std::shared_ptr<engine::Object> &object);
    static void DrawGameObject(const std::shared_ptr<engine::GameObject> &game_object);
    static void DrawComponent(const std::shared_ptr<engine::Component> &component);
    static void DrawInspectable(const std::shared_ptr<engine::Inspectable> &inspectable);
    static void DrawAssetHierarchy(const std::shared_ptr<engine::AssetHierarchy> &asset_hierarchy);

public:
    std::string Name() override;
    void OnEditorGui() override;
};
}