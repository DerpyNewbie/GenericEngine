// ReSharper disable CppExplicitSpecializationInNonNamespaceScope
#pragma once
#include "game_object.h"
#include "Asset/asset_ptr.h"

#include <shtypes.h>

namespace engine
{
using FilterSpec = COMDLG_FILTERSPEC;
using namespace engine;

// NOTE(derpy): apply std-like naming style 
// ReSharper disable CppInconsistentNaming
template <typename T>
static constexpr bool is_asset_ptr = false;

template <typename T> requires std::is_base_of_v<IAssetPtr, T>
static constexpr bool is_asset_ptr<T> = true;

template <typename T>
static constexpr bool is_inspectable = false;

template <typename T> requires std::is_same_v<decltype(std::declval<T>().OnInspectorGui()), void>
static constexpr bool is_inspectable<T> = true;
// ReSharper restore CppInconsistentNaming

class Gui
{
public:
    struct DragDropTarget
    {
        static constexpr auto kObject = "ENGINE_OBJECT";
        static constexpr auto kObjectGuid = "ENGINE_OBJ_GUID";
    };

    static bool OpenFileDialog(std::string &file_path, const std::vector<FilterSpec> &filters = {});

    static bool SaveFileDialog(std::string &file_path, const std::string &default_name,
                               const std::vector<FilterSpec> &filters = {});

    static void SetObjectDragDropTarget(const std::shared_ptr<Object> &object);
    static void SetObjectDragDropTarget(xg::Guid guid);

    static std::shared_ptr<Object> GetObjectDragDropTarget(const ImGuiPayload *payload);
    static std::shared_ptr<Object> GetObjectDragDropTarget();

    static ImVec2 GetFieldRect();

    template <typename T>
    static std::shared_ptr<T> MakeCompatible(std::shared_ptr<Object> object);

    template <typename T> requires std::is_base_of_v<Component, T>
    static std::shared_ptr<T> MakeCompatible(std::shared_ptr<Object> object);

    template <typename T>
    static bool AssetDragDropTarget(IAssetPtr &asset_ptr);

    template <typename T>
    static bool AssetPicker(IAssetPtr &asset_ptr);

    static bool BoolField(const char *label, bool &value);

    static bool FloatField(const char *label, float &value);

    static bool IntField(const char *label, int &value);

    static bool Vector2Field(const char *label, Vector2 &value);

    static bool Vector3Field(const char *label, Vector3 &value);

    static bool QuaternionField(const char *label, Quaternion &value);

    static bool ColorField(const char *label, Color &value);

    /// base implementation
    template <typename T>
    static bool PropertyField(const char *label, T &value)
    {
        ImGui::Text("PropertyField for type '%s' is not supported.", typeid(T).name());
        return false;
    }

    /// int specialization
    template <>
    static bool PropertyField(const char *label, int &value);

    /// float specialization
    template <>
    static bool PropertyField(const char *label, float &value);

    /// bool specialization
    template <>
    static bool PropertyField(const char *label, bool &value);

    /// Vector2 specialization
    template <>
    static bool PropertyField(const char *label, Vector2 &value);

    /// Vector3 specialization
    template <>
    static bool PropertyField(const char *label, Vector3 &value);

    /// Quaternion specialization
    template <>
    static bool PropertyField(const char *label, Quaternion &value);

    /// Color specialization
    template <>
    static bool PropertyField(const char *label, Color &value);

    /// IAssetPtr specialization
    template <typename T>
    static bool PropertyField(const char *label, IAssetPtr &value);

    /// AssetPtr<T> specialization
    template <typename T>
    static bool PropertyField(const char *label, AssetPtr<T> &value);

    /// std::pair<K, V> specialization
    template <typename K, typename V>
    static bool PropertyField(const char *label, std::pair<K, V> &value);

    /// std::vector<T> specialization
    template <typename T>
    static bool PropertyField(const char *label, std::vector<T> &value);

    template <typename T> requires is_inspectable<T>
    static bool ExpandablePropertyField(const char *label, IAssetPtr &value);

    template <typename T> requires is_inspectable<T>
    static bool ExpandablePropertyField(const char *label, AssetPtr<T> &value);
};

template <typename T>
std::shared_ptr<T> Gui::MakeCompatible(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<T>(object);
}

template <typename T> requires std::is_base_of_v<Component, T>
std::shared_ptr<T> Gui::MakeCompatible(const std::shared_ptr<Object> object)
{
    auto casted = std::dynamic_pointer_cast<T>(object);
    if (casted != nullptr)
        return casted;

    const auto game_object = std::dynamic_pointer_cast<GameObject>(object);
    if (game_object != nullptr)
        return game_object->GetComponent<T>();

    return nullptr;
}

template <typename T>
bool Gui::AssetDragDropTarget(IAssetPtr &asset_ptr)
{
    if (!ImGui::BeginDragDropTarget())
    {
        return false;
    }

    const auto payload = ImGui::GetDragDropPayload();
    if (!payload->IsDataType(DragDropTarget::kObjectGuid))
    {
        ImGui::EndDragDropTarget();
        return false;
    }

    const auto object = GetObjectDragDropTarget(payload);

    auto casted_object = MakeCompatible<T>(object);
    if (casted_object == nullptr)
    {
        ImGui::Text("Type mismatch. Requires '%s'", typeid(T).name());
        ImGui::EndDragDropTarget();
        return false;
    }

    if (ImGui::AcceptDragDropPayload(DragDropTarget::kObjectGuid))
    {
        asset_ptr = IAssetPtr::FromManaged(casted_object);

        ImGui::EndDragDropTarget();
        return true;
    }

    ImGui::EndDragDropTarget();
    return false;
}

template <typename T>
bool Gui::AssetPicker(IAssetPtr &asset_ptr)
{
    static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");

    auto objects = Object::FindByType<T>();
    if (objects.empty())
    {
        ImGui::Text("There was no objects with type '%s'", typeid(T).name());
        return false;
    }

    if (ImGui::MenuItem("None"))
    {
        asset_ptr = IAssetPtr{};
        ImGui::CloseCurrentPopup();
        return true;
    }

    for (auto &object : objects)
    {
        auto obj_asset_ptr = IAssetPtr::FromManaged(object);
        if (ImGui::MenuItem(obj_asset_ptr.Name().c_str()))
        {
            asset_ptr = obj_asset_ptr;
            ImGui::CloseCurrentPopup();
            return true;
        }

        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(("GUID: " + object->Guid().str()).c_str());
            ImGui::EndTooltip();
        }
    }
    return false;
}

template <typename T>
bool Gui::PropertyField(const char *label, IAssetPtr &value)
{
    static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");

    bool has_changed = false;
    ImGui::PushID(label);
    {
        if (ImGui::BeginPopup("##PROPERTY_FIELD_POPUP"))
        {
            has_changed |= AssetPicker<T>(value);
            ImGui::EndPopup();
        }

        if (ImGui::Button(value.Name().c_str(), GetFieldRect()))
        {
            ImGui::OpenPopup("##PROPERTY_FIELD_POPUP");
        }

        has_changed |= AssetDragDropTarget<T>(value);

        ImGui::SameLine();
        ImGui::Text(label);
    }
    ImGui::PopID();

    return has_changed;
}

template <typename T>
bool Gui::PropertyField(const char *label, AssetPtr<T> &value)
{
    return PropertyField<T>(label, static_cast<IAssetPtr &>(value));
}

template <typename K, typename V>
bool Gui::PropertyField(const char *label, std::pair<K, V> &value)
{
    bool result = false;
    ImGui::PushID(label);
    {
        if (ImGui::CollapsingHeader(label))
        {
            ImGui::Indent();
            result |= PropertyField("Key", value.first);
            result |= PropertyField("Value", value.second);
            ImGui::Unindent();
        }
    }
    ImGui::PopID();
    return result;
}

template <typename T>
bool Gui::PropertyField(const char *label, std::vector<T> &value)
{
    bool result = false;
    ImGui::PushID(label);
    {
        if (ImGui::CollapsingHeader(label))
        {
            ImGui::Indent();

            int size = value.size();
            if (ImGui::InputInt("Size", &size))
            {
                value.resize(max(size, 0));
                result = true;
            }

            for (int i = 0; i < value.size(); ++i)
            {
                ImGui::PushID(i);
                result |= PropertyField(("Element " + std::to_string(i)).c_str(), value[i]);
                ImGui::PopID();
            }
            ImGui::Unindent();
        }
    }
    ImGui::PopID();
    return result;
}

template <typename T> requires is_inspectable<T>
bool Gui::ExpandablePropertyField(const char *label, IAssetPtr &value)
{
    const bool result = PropertyField<T>(label, value);
    ImGui::PushID(label);
    if (ImGui::CollapsingHeader(label))
    {
        ImGui::Indent();
        const auto locked = std::dynamic_pointer_cast<T>(value.Lock());
        if (locked == nullptr)
        {
            ImGui::Text("There is nothing to show...");
        }
        else
        {
            locked->OnInspectorGui();
        }
        ImGui::Unindent();
    }
    ImGui::PopID();
    return result;
}

template <typename T> requires is_inspectable<T>
bool Gui::ExpandablePropertyField(const char *label, AssetPtr<T> &value)
{
    return ExpandablePropertyField<T>(label, static_cast<IAssetPtr &>(value));
}
}