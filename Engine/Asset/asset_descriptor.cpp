#include "pch.h"

#include "asset_descriptor.h"

#include "asset_database.h"
#include "logger.h"
#include "Importer/asset_importer.h"

namespace engine
{
using namespace rapidjson;

path AssetDescriptor::GetAbsoluteAssetFilePath(const path &asset_path)
{
    auto absolute_path = asset_path;
    if (absolute_path.is_relative())
        absolute_path = AssetDatabase::GetProjectDirectory() / absolute_path;

    auto absolute_path_str = absolute_path.string();
    do
    {
        if (absolute_path_str.ends_with(kMetaFileExtension))
        {
            absolute_path_str = absolute_path_str.substr(0, absolute_path_str.size() - 5);
        }

        const auto trailing = absolute_path_str.back();
        if (trailing == '/' || trailing == '\\')
        {
            absolute_path_str = absolute_path_str.substr(0, absolute_path_str.size() - 1);
        }
    } while (absolute_path_str.ends_with(kMetaFileExtension) ||
             absolute_path_str.back() == '/' || absolute_path_str.back() == '\\');

    return absolute_path_str;
}

path AssetDescriptor::GetMetaFilePath(const path &asset_path)
{
    const auto absolute_asset_path = GetAbsoluteAssetFilePath(asset_path);
    return absolute_asset_path.string() + kMetaFileExtension;
}

bool AssetDescriptor::GetMetaJson(const path &asset_path, std::string &out_json)
{
    const auto descriptor_file_path = GetMetaFilePath(asset_path);
    if (!exists(descriptor_file_path))
    {
        return false;
    }

    auto input_stream = std::ifstream(descriptor_file_path);
    out_json = std::string(std::istreambuf_iterator(input_stream), std::istreambuf_iterator<char>());
    return true;
}

void AssetDescriptor::Write(const path &path)
{
    StringBuffer string_buffer;
    PrettyWriter writer(string_buffer);
    writer.StartObject();

    writer.String(kGuidKey);
    writer.String(guid.str().c_str());

    writer.String(kTypeKey);
    writer.String(type_hint.c_str());

    writer.String(kDataKey);
    auto data_member = m_meta_json_.FindMember(kDataKey);
    if (data_member == m_meta_json_.MemberEnd())
    {
        m_meta_json_.AddMember(StringRef(kDataKey),
                               Value(kObjectType),
                               m_meta_json_.GetAllocator());
        data_member = m_meta_json_.FindMember(kDataKey);
    }
    data_member->value.Accept(writer);

    writer.EndObject();

    std::ofstream output_stream(path);
    output_stream << string_buffer.GetString();
    output_stream.close();
}
GenericValue<UTF8<>> &AssetDescriptor::GetDataValue()
{
    return m_meta_json_.FindMember(kDataKey)->value;
}

#pragma push_macro("GetObject")
#undef GetObject

std::shared_ptr<AssetDescriptor> AssetDescriptor::Read(const path &path)
{
    std::string meta_json;
    const std::string meta_file_path = GetMetaFilePath(path).string();
    const std::string asset_file_path = GetAbsoluteAssetFilePath(path).string();

    if (!GetMetaJson(path, meta_json))
    {
        Logger::Log<AssetDescriptor>("No meta file `%s` found. Generating!", path.string().c_str());
        const auto descriptor = Instantiate<AssetDescriptor>(path.string().c_str());
        descriptor->path_hint = asset_file_path;
        descriptor->guid = xg::newGuid();
        descriptor->type_hint = path.extension().string();

        descriptor->m_meta_json_.SetObject();
        auto &a = descriptor->m_meta_json_.GetAllocator();

        auto guid_str = descriptor->guid.str();
        auto guid_value = Value();
        guid_value.SetString(guid_str.c_str(), guid_str.size(), a);

        descriptor->m_meta_json_.AddMember(StringRef(kGuidKey), guid_value, a);

        auto type_hint_str = descriptor->type_hint;
        auto type_hint_value = Value();
        type_hint_value.SetString(type_hint_str.c_str(), type_hint_str.size(), a);

        descriptor->m_meta_json_.AddMember(StringRef(kTypeKey), type_hint_value, a);
        descriptor->m_meta_json_.AddMember(StringRef(kDataKey), Value(kObjectType), a);
        descriptor->Write(meta_file_path);
        if (!GetMetaJson(path, meta_json))
        {
            Logger::Error<AssetDescriptor>("Failed to generate meta file for asset `%s`", path.string().c_str());
            return nullptr;
        }
    }

    auto result = Instantiate<AssetDescriptor>(meta_file_path);
    result->path_hint = asset_file_path;

    Reload(result, meta_json);
    return result;
}

void AssetDescriptor::Reload(const std::shared_ptr<AssetDescriptor> &instance, const std::string &json)
{
    instance->m_meta_json_.Parse(json.c_str());
    instance->guid = xg::Guid(instance->m_meta_json_.FindMember(kGuidKey)->value.GetString());
    instance->type_hint = instance->m_meta_json_.FindMember(kTypeKey)->value.GetString();

    const auto asset_importer = AssetImporter::Get(instance->type_hint);
    if (asset_importer == nullptr)
    {
        Logger::Warn<AssetDescriptor>("Asset importer for type '%s' not found! Will ignore file '%s'",
                                      instance->type_hint.c_str(), instance->path_hint.c_str());
        return;
    }

    instance->managed_object = asset_importer->Import(instance.get());
}

#pragma pop_macro("GetObject")

std::vector<std::string> AssetDescriptor::GetKeys()
{
    const auto &data = GetDataValue();
    std::vector<std::string> keys;
    for (auto i = data.MemberBegin(); i != data.MemberEnd(); ++i)
    {
        keys.emplace_back(i->name.GetString());
    }

    return keys;
}
void AssetDescriptor::ClearKeys()
{
    auto &data = GetDataValue();
    data.RemoveAllMembers();
}

void AssetDescriptor::SetString(const std::string &key, const std::string &value)
{
    auto json_key = Value(key.c_str(), key.size(), m_meta_json_.GetAllocator());
    auto json_value = Value(value.c_str(), value.size(), m_meta_json_.GetAllocator());
    GetDataValue().AddMember(json_key, json_value, m_meta_json_.GetAllocator());
}

void AssetDescriptor::SetInt(const std::string &key, const int value)
{
    auto json_key = Value(key.c_str(), key.size(), m_meta_json_.GetAllocator());
    auto json_value = Value(value);
    GetDataValue().AddMember(json_key, json_value, m_meta_json_.GetAllocator());
}

void AssetDescriptor::SetFloat(const std::string &key, const float value)
{
    auto json_key = Value(key.c_str(), key.size(), m_meta_json_.GetAllocator());
    auto json_value = Value(value);
    GetDataValue().AddMember(json_key, json_value, m_meta_json_.GetAllocator());
}

void AssetDescriptor::SetBool(const std::string &key, const bool value)
{
    auto json_key = Value(key.c_str(), key.size(), m_meta_json_.GetAllocator());
    auto json_value = Value(value);
    GetDataValue().AddMember(json_key, json_value, m_meta_json_.GetAllocator());
}

std::string AssetDescriptor::GetString(const std::string &key)
{
    auto &data = GetDataValue();
    const auto json_value = data.FindMember(StringRef(key.c_str(), key.size()));
    if (json_value == data.MemberEnd())
    {
        return "";
    }

    return json_value->value.GetString();
}

int AssetDescriptor::GetInt(const std::string &key)
{
    auto &data = GetDataValue();
    const auto json_value = data.FindMember(StringRef(key.c_str(), key.size()));
    if (json_value == data.MemberEnd())
    {
        return 0;
    }

    return json_value->value.GetInt();
}

float AssetDescriptor::GetFloat(const std::string &key)
{
    auto &data = GetDataValue();
    const auto json_value = data.FindMember(StringRef(key.c_str(), key.size()));
    if (json_value == data.MemberEnd())
    {
        return 0.0f;
    }

    return json_value->value.GetFloat();
}

bool AssetDescriptor::GetBool(const std::string &key)
{
    auto &data = GetDataValue();
    const auto json_value = data.FindMember(StringRef(key.c_str(), key.size()));
    if (json_value == data.MemberEnd())
    {
        return false;
    }

    return json_value->value.GetBool();
}

IAssetPtr AssetDescriptor::ToAssetPtr()
{
    return IAssetPtr::FromAssetDescriptor(shared_from_base<AssetDescriptor>());
}

void AssetDescriptor::Save()
{
    if (IsInternalAsset())
    {
        Logger::Warn<AssetDescriptor>("Cannot save internal asset '%s'", guid.str().c_str());
        return;
    }

    std::string json;
    Write(GetMetaFilePath(path_hint));
}

void AssetDescriptor::Reload()
{
    if (IsInternalAsset())
    {
        Logger::Warn<AssetDescriptor>("Cannot reload internal asset '%s'", guid.str().c_str());
        return;
    }

    std::string json;
    GetMetaJson(path_hint, json);
    Reload(shared_from_base<AssetDescriptor>(), json);
}
bool AssetDescriptor::IsInternalAsset() const
{
    return path_hint.string() == kInternalAssetPath;
}
}