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

/// <summary>
/// ImGuiを使ったInspector用のGuiやダイアログなどの便利関数をまとめたクラスです。
/// </summary>
class Gui
{
public:
    /// <summary>
    /// Drag & Dropで使うPayloadの名前です。
    /// </summary>
    struct DragDropTarget
    {
        static constexpr auto kObject = "ENGINE_OBJECT";
        static constexpr auto kObjectGuid = "ENGINE_OBJ_GUID";
    };

    /// <summary>
    /// MessageBoxのダイアログに表示するアイコンの種類です。
    /// </summary>
    enum class MbDialogIcon
    {
        kNone,
        kInfo,
        kWarning,
        kHelp,
        kError
    };

    /// <summary>
    /// MessageBoxのダイアログの設定です。
    /// </summary>
    struct MbDialogOption
    {
        MbDialogIcon icon = MbDialogIcon::kNone;

        /// <summary>
        /// iconに対応するMessageBoxのflagを取得します。
        /// </summary>
        UINT Flags() const;
    };

    /// <summary>
    /// 表示するものがないことをInspectorに表示するためのInspectableです。
    /// </summary>
    struct NothingToShowInspectable : Inspectable
    {
        void OnInspectorGui() override;
    };

    /// <summary>
    /// ファイルを開くDialogを表示します。
    /// </summary>
    /// <param name="file_path">最初に開くフォルダのPath。選択されたファイルのPathが書き込まれます。</param>
    /// <param name="filters">表示するファイルの種類</param>
    /// <returns>ファイルが選択された場合 true</returns>
    static bool OpenFileDialog(std::string &file_path, const std::vector<FilterSpec> &filters = {});

    /// <summary>
    /// ファイルを保存するDialogを表示します。
    /// </summary>
    /// <param name="file_path">選択された保存先のPathが書き込まれます。</param>
    /// <param name="default_name">最初に入力されているファイル名</param>
    /// <param name="filters">表示するファイルの種類</param>
    /// <returns>保存先が選択された場合 true</returns>
    static bool SaveFileDialog(
        std::string &file_path, const std::string &default_name,
        const std::vector<FilterSpec> &filters = {}
    );

    /// <summary>
    /// OKボタンのみのMessageBoxを表示します。
    /// </summary>
    /// <param name="title">タイトル</param>
    /// <param name="content">本文</param>
    /// <param name="options">アイコンなどの設定</param>
    /// <returns>OKが押された場合 true</returns>
    static bool OkDialog(const std::string &title, const std::string &content, MbDialogOption options = {});
    /// <summary>
    /// OKとキャンセルのボタンがあるMessageBoxを表示します。
    /// </summary>
    /// <param name="title">タイトル</param>
    /// <param name="content">本文</param>
    /// <param name="options">アイコンなどの設定</param>
    /// <returns>OKが押された場合 true</returns>
    static bool OkCancelDialog(const std::string &title, const std::string &content, MbDialogOption options = {});

    /// <summary>
    /// Objectの折りたたみ可能なHeaderを表示し、DragDropのSourceにします。
    /// </summary>
    /// <param name="object">対象のObject</param>
    /// <param name="name">表示する名前。空の場合はNameOf(object)が使われます。</param>
    /// <returns>Headerが開かれている場合 true</returns>
    static bool ObjectHeader(const std::shared_ptr<Object> &object, std::string name = "");
    /// <summary>
    /// 直前に表示したItemを、ObjectのDragDropのSourceにします。
    /// </summary>
    /// <param name="object">Dragで渡されるObject</param>
    static void MakeDragDropSource(const std::shared_ptr<Object> &object);

    /// <summary>
    /// ObjectのGuidをDragDropのPayloadに設定し、名前をDrag中の表示に追加します。
    /// </summary>
    /// <param name="object">Dragで渡されるObject</param>
    static void SetDragDropPayload(const std::shared_ptr<Object> &object);
    /// <summary>
    /// GuidをDragDropのPayloadに設定し、Guidをドラッグ中の表示に追加します。
    /// </summary>
    /// <param name="guid">Dragで渡されるObjectのGuid</param>
    static void SetDragDropPayload(xg::Guid guid);

    /// <summary>
    /// PayloadのGuidからObjectを取得します。まだ読み込まれていないAssetの場合はImportします。
    /// </summary>
    /// <param name="payload">DragDropのPayload</param>
    /// <returns>見つからない場合 nullptr</returns>
    static std::shared_ptr<Object> GetDragDropPayload(const ImGuiPayload *payload);
    /// <summary>
    /// 現在DragされているPayloadからObjectを取得します。
    /// </summary>
    /// <returns>見つからない場合 nullptr</returns>
    static std::shared_ptr<Object> GetDragDropPayload();

    /// <summary>
    /// Objectの表示用の名前を取得します。Componentの場合は"GameObject名 (Component名)"、Assetの場合は"名前 (ファイル名)"になります。
    /// </summary>
    /// <param name="object">対象のObject</param>
    static std::string NameOf(const std::shared_ptr<Object> &object);

    /// <summary>
    /// Fieldの標準の大きさ(幅、高さ)を取得します。
    /// </summary>
    static ImVec2 GetFieldRect();

    /// <summary>
    /// Objectを T にCastします。
    /// </summary>
    /// <returns>Castできない場合 nullptr</returns>
    template <typename T>
    static std::shared_ptr<T> MakeCompatible(std::shared_ptr<Object> object);

    /// <summary>
    /// Objectを T にCastします。Castできず、ObjectがGameObjectである場合は、そのGameObjectが持つ T のComponentを取得します。
    /// </summary>
    /// <returns>見つからない場合 nullptr</returns>
    template <typename T> requires std::is_base_of_v<Component, T>
    static std::shared_ptr<T> MakeCompatible(std::shared_ptr<Object> object);

    /// <summary>
    /// 直前に表示したItemを T のObjectのDropTargetにします。
    /// </summary>
    /// <param name="asset_ptr">Dropされた時に書き換えられるAssetPtr</param>
    /// <returns>Dropされてasset_ptrが変更された場合 true</returns>
    template <typename T>
    static bool AssetDragDropTarget(IAssetPtr &asset_ptr);

    /// <summary>
    /// T のObjectの一覧をMenuとして表示し、選択されたものをasset_ptrに設定します。
    /// </summary>
    /// <param name="asset_ptr">選択された時に書き換えられるAssetPtr</param>
    /// <returns>asset_ptrが変更された場合 true</returns>
    template <typename T>
    static bool AssetPicker(IAssetPtr &asset_ptr);

    /// <summary>
    /// 編集できない文字列のFieldを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">表示する文字列</param>
    static void ReadOnlyStringField(const char *label, const std::string &value);

    /// <summary>
    /// boolを編集するCheckboxを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
    static bool BoolField(const char *label, bool &value);

    /// <summary>
    /// floatをDragで編集するFieldを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
    static bool FloatField(const char *label, float &value);

    /// <summary>
    /// intをDragで編集するFieldを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
    static bool IntField(const char *label, int &value);

    /// <summary>
    /// Vector2をDragで編集するFieldを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
    static bool Vector2Field(const char *label, Vector2 &value);

    /// <summary>
    /// Vector3をDragで編集するFieldを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
    static bool Vector3Field(const char *label, Vector3 &value);

    /// <summary>
    /// Quaternionをオイラー角(度)として編集するFieldを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
    static bool QuaternionField(const char *label, Quaternion &value);

    /// <summary>
    /// Colorを編集するColorPickerを表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集する値</param>
    /// <returns>値が変更された場合 true</returns>
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

    /// <summary>
    /// AssetのPropertyFieldに加えて、参照先のOnInspectorGuiを折りたたみ表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集するAssetPtr</param>
    /// <returns>値が変更された場合 true</returns>
    template <typename T> requires is_inspectable<T>
    static bool ExpandablePropertyField(const char *label, IAssetPtr &value);

    /// <summary>
    /// AssetのPropertyFieldに加えて、参照先のOnInspectorGuiを折りたたみ表示します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="value">編集するAssetPtr</param>
    /// <returns>値が変更された場合 true</returns>
    template <typename T> requires is_inspectable<T>
    static bool ExpandablePropertyField(const char *label, AssetPtr<T> &value);

    /// <summary>
    /// 折りたたみ可能なHeaderを表示し、開かれている場合にinspectableのOnInspectorGuiを呼び出します。
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="inspectable">表示する対象</param>
    template <typename T> requires is_inspectable<T>
    static void Expand(const char *label, T *inspectable);
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

    const auto object = GetDragDropPayload(payload);

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
        ImGui::PushID(object.get());

        auto obj_asset_ptr = IAssetPtr::FromManaged(object);
        if (ImGui::MenuItem(NameOf(object).c_str()))
        {
            asset_ptr = obj_asset_ptr;
            ImGui::PopID();
            ImGui::CloseCurrentPopup();
            return true;
        }

        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(("GUID: " + object->Guid().str()).c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopID();
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
    auto inspectable = std::dynamic_pointer_cast<T>(value.Lock());
    if (inspectable == nullptr)
    {
        static NothingToShowInspectable nothing_to_show;
        Expand(label, &nothing_to_show);
    }
    else
    {
        Expand(label, inspectable.get());
    }

    return result;
}

template <typename T> requires is_inspectable<T>
bool Gui::ExpandablePropertyField(const char *label, AssetPtr<T> &value)
{
    return ExpandablePropertyField<T>(label, static_cast<IAssetPtr &>(value));
}

template <typename T> requires is_inspectable<T>
void Gui::Expand(const char *label, T *inspectable)
{
    ImGui::PushID(label);
    if (ImGui::CollapsingHeader(label))
    {
        ImGui::Indent();
        inspectable->OnInspectorGui();
        ImGui::Unindent();
    }
    ImGui::PopID();
}
}