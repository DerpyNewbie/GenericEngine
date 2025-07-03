#pragma once
#include "logger.h"
#include "object.h"
#include "Asset/asset_database.h"
#include "Asset/asset_hierarchy.h"
#include "Asset/asset_ptr.h"

namespace editor
{
class Gui
{
public:
    struct DragDropTarget
    {
        static constexpr auto kObject = "ENGINE_OBJECT";
        static constexpr auto kAsset = "ENGINE_ASSET";
    };

    template <typename T>
    static bool PropertyField(const char *label, engine::IAssetPtr &value, std::string type_hint);
};

template <typename T>
bool Gui::PropertyField(const char *label, engine::IAssetPtr &value, std::string type_hint)
{
    static_assert(std::is_base_of<engine::Object, T>() && "Property must inherit from engine::Object");

    ImGui::PushID(&value);
    if (ImGui::BeginPopup("##PROPERTY_FIELD_POPUP"))
    {
        auto assets = engine::AssetDatabase::GetAssetsByType(type_hint);
        if (assets.empty())
        {
            ImGui::Text("No assets found");
        }
        else
        {
            for (auto &asset : assets)
            {
                auto name = asset.IsLoaded() ? asset.Lock()->Name() + ": " + asset.Guid().str() : asset.Guid().str();
                if (ImGui::Button(name.c_str()))
                {
                    value = asset;
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::EndPopup();
    }

    auto name = value.IsLoaded() ? value.Lock()->Name() + ": " + value.Guid().str() : value.Guid().str();
    if (ImGui::Button(value.Guid().str().c_str()))
    {
        ImGui::OpenPopup("##PROPERTY_FIELD_POPUP");
    }
    ImGui::SameLine();
    ImGui::Text(label);
    ImGui::PopID();
    if (ImGui::BeginDragDropTarget())
    {
        if (const auto payload = ImGui::AcceptDragDropPayload(DragDropTarget::kAsset))
        {
            engine::Logger::Log<Gui>("PropertyField: Applied field");
            value = engine::AssetDatabase::GetAsset(static_cast<engine::AssetHierarchy *>(payload->Data)->asset->guid);
        }
    }

    return false;
}
}