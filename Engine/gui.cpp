#include "pch.h"
#include "gui.h"

#include "str_util.h"
#include "Asset/asset_database.h"

#include <shobjidl_core.h>

namespace engine
{
bool Gui::OpenFileDialog(std::string &file_path, const std::vector<FilterSpec> &filters)
{
    IFileOpenDialog *p_file_open;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                  IID_IFileOpenDialog, reinterpret_cast<void **>(&p_file_open));

    if (FAILED(hr))
    {
        return false;
    }

    if (!filters.empty())
    {
        hr = p_file_open->SetFileTypes(filters.size(), filters.data());
        if (FAILED(hr))
        {
            Logger::Error<Gui>("Failed to set file types");
        }
    }

    IShellItem *p_default_folder = nullptr;
    hr = SHCreateItemFromParsingName(StringUtil::ConvertToWString(file_path).c_str(), nullptr,
                                     IID_PPV_ARGS(&p_default_folder));
    if (SUCCEEDED(hr))
    {
        hr = p_file_open->SetDefaultFolder(p_default_folder);
        p_default_folder->Release();
        p_default_folder = nullptr;
    }
    else
    {
        Logger::Error<Gui>("Failed to set default name");
    }

    hr = p_file_open->Show(nullptr);
    if (FAILED(hr))
    {
        Logger::Error<Gui>("File open cancelled");
        return false;
    }

    IShellItem *p_item;
    hr = p_file_open->GetResult(&p_item);
    if (FAILED(hr))
    {
        Logger::Error<Gui>("Failed to get result from shell item");
        return false;
    }

    PWSTR path;
    hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &path);
    if (FAILED(hr))
    {
        Logger::Error<Gui>("Failed to get display name from shell item");
        return false;
    }

    file_path = StringUtil::Utf16ToUtf8(std::wstring(path));
    CoTaskMemFree(path);
    return true;

}

bool Gui::SaveFileDialog(std::string &file_path, const std::string &default_name,
                         const std::vector<FilterSpec> &filters)
{
    IFileSaveDialog *p_file_save;

    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL,
                                  IID_IFileSaveDialog, reinterpret_cast<void **>(&p_file_save));

    if (FAILED(hr))
    {
        return false;
    }

    hr = p_file_save->SetFileName(StringUtil::Utf8ToUtf16(default_name).c_str());
    if (FAILED(hr))
    {
        Logger::Error<Gui>("Failed to set default name");
    }

    hr = p_file_save->SetFileTypes(filters.size(), filters.data());
    if (FAILED(hr))
    {
        Logger::Error<Gui>("Failed to set file types");
    }

    hr = p_file_save->Show(nullptr);
    if (FAILED(hr))
    {
        Logger::Error<Gui>("File save cancelled");
        return false;
    }

    IShellItem *p_item;
    hr = p_file_save->GetResult(&p_item);
    if (FAILED(hr))
    {
        Logger::Error<Gui>("Failed to get result from shell item");
        return false;
    }

    PWSTR path;
    hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &path);
    if (FAILED(hr))
    {
        Logger::Error<Gui>("Failed to get display name from shell item");
        return false;
    }

    file_path = StringUtil::Utf16ToUtf8(std::wstring(path));
    CoTaskMemFree(path);
    return true;
}

void Gui::SetObjectDragDropTarget(const std::shared_ptr<Object> &object)
{
    SetObjectDragDropTarget(object->Guid());
}

void Gui::SetObjectDragDropTarget(const xg::Guid guid)
{
    const auto guid_str = guid.str();
    ImGui::SetDragDropPayload(DragDropTarget::kObjectGuid, guid_str.c_str(), guid_str.size() + 1);
}

std::shared_ptr<Object> Gui::GetObjectDragDropTarget(const ImGuiPayload *payload)
{
    if (payload == nullptr)
    {
        return nullptr;
    }

    if (!payload->IsDataType(DragDropTarget::kObjectGuid))
    {
        return nullptr;
    }

    // search for instantiated objects
    const auto guid_str = std::string(static_cast<char *>(payload->Data));
    const auto guid = xg::Guid(guid_str);
    const auto object = Object::Find(guid);

    if (object != nullptr)
    {
        return object;
    }

    // search for unloaded AssetDatabase assets
    const auto asset_descriptor = AssetDatabase::GetAssetDescriptor(guid);
    if (asset_descriptor == nullptr)
    {
        return nullptr;
    }

    if (asset_descriptor->managed_object == nullptr)
    {
        asset_descriptor->Reload();
    }

    if (asset_descriptor->managed_object == nullptr)
    {
        return nullptr;
    }

    return asset_descriptor->managed_object;
}

std::shared_ptr<Object> Gui::GetObjectDragDropTarget()
{
    return GetObjectDragDropTarget(ImGui::GetDragDropPayload());
}

ImVec2 Gui::GetFieldRect()
{
    const auto height = ImGui::GetTextLineHeightWithSpacing();
    const auto width = ImGui::GetContentRegionAvail().x;
    return ImVec2(width * 0.7F - 0.25F, height);
}

bool Gui::BoolField(const char *label, bool &value)
{
    return ImGui::Checkbox(label, &value);
}

bool Gui::FloatField(const char *label, float &value)
{
    return ImGui::DragFloat(label, &value, 0.01F, 0.0F, 0.0F, "%.2f");
}

bool Gui::IntField(const char *label, int &value)
{
    return ImGui::DragInt(label, &value, 1.0F, 0.0F, 0.0F, "%d");
}
}