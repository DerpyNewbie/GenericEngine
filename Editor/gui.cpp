#include "pch.h"
#include "gui.h"

#include "engine_util.h"
#include "str_util.h"

#include <shobjidl_core.h>

bool editor::Gui::OpenFileDialog(std::string &file_path, const std::vector<FilterSpec> &filters)
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
            engine::Logger::Error<Gui>("Failed to set file types");
        }
    }

    hr = p_file_open->Show(nullptr);
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("Failed to show file open dialog");
        return false;
    }

    IShellItem *p_item;
    hr = p_file_open->GetResult(&p_item);
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("Failed to get result from shell item");
        return false;
    }

    PWSTR path;
    hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &path);
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("Failed to get display name from shell item");
        return false;
    }

    file_path = StringUtil::Utf16ToUtf8(std::wstring(path));
    CoTaskMemFree(path);
    return true;

}
bool editor::Gui::SaveFileDialog(std::string &file_path, const std::string &default_name,
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
        engine::Logger::Error<Gui>("Failed to set default name");
    }

    hr = p_file_save->SetFileTypes(filters.size(), filters.data());
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("Failed to set file types");
    }

    hr = p_file_save->Show(nullptr);
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("File save cancelled");
        return false;
    }

    IShellItem *p_item;
    hr = p_file_save->GetResult(&p_item);
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("Failed to get result from shell item");
        return false;
    }

    PWSTR path;
    hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &path);
    if (FAILED(hr))
    {
        engine::Logger::Error<Gui>("Failed to get display name from shell item");
        return false;
    }

    file_path = StringUtil::Utf16ToUtf8(std::wstring(path));
    CoTaskMemFree(path);
    return true;
}