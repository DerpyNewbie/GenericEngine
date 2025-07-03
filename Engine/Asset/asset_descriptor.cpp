#include "pch.h"

#include "asset_descriptor.h"

#include "asset_database.h"
#include "logger.h"
#include "serializer.h"
#include "Importer/asset_importer.h"

namespace engine
{

path AssetDescriptor::ToMetaFilePath(const std::filesystem::path &path)
{
    auto normalized_path = path;
    if (normalized_path.is_relative())
    {
        normalized_path = AssetDatabase::GetProjectDirectory() / normalized_path;
    }

    const auto trailing = path.string().back();
    if (trailing == '/' || trailing == '\\')
    {
        normalized_path = path.string().substr(0, path.string().size() - 1);
    }

    return normalized_path.string() + ".meta";
}

void AssetDescriptor::Write(const std::filesystem::path &path) const
{
    rapidjson::StringBuffer string_buffer;
    rapidjson::PrettyWriter writer(string_buffer);
    writer.StartObject();

    writer.String("guid");
    writer.String(guid.str().c_str());

    writer.String("type");
    writer.String(type_hint.c_str());

    // TODO: implement embed custom data
    writer.String("data");
    writer.StartArray();

    writer.EndArray();

    writer.EndObject();

    std::ofstream output_stream(path);
    output_stream << string_buffer.GetString();
    output_stream.close();
}

std::shared_ptr<AssetDescriptor> AssetDescriptor::Read(const std::filesystem::path &path)
{
    const auto descriptor_file_path = ToMetaFilePath(path);
    auto absolute_path = path;
    if (absolute_path.is_relative())
        absolute_path = AssetDatabase::GetProjectDirectory() / absolute_path;

    if (!exists(descriptor_file_path))
    {
        Logger::Log<AssetDescriptor>("No meta file `%s` found. Generating!", path.string().c_str());
        auto descriptor = Instantiate<AssetDescriptor>(path.string().c_str());
        descriptor->path = absolute_path;
        descriptor->guid = xg::newGuid();
        descriptor->type_hint = path.extension().string();
        descriptor->Write(descriptor_file_path);
    }

    auto input_stream = std::ifstream(descriptor_file_path);
    auto json_str = std::string((std::istreambuf_iterator(input_stream)), std::istreambuf_iterator<char>());
    auto result = Instantiate<AssetDescriptor>(path.string().c_str());

    rapidjson::Document document;
    document.Parse(json_str.c_str());
    result->path = absolute_path;
    result->guid = xg::Guid(document.FindMember("guid")->value.GetString());
    result->type_hint = document.FindMember("type")->value.GetString();

    auto asset_importer = AssetImporter::Get(result->type_hint);
    if (asset_importer == nullptr)
    {
        Logger::Warn<AssetDescriptor>("Asset importer for type '%s' not found! Will ignore file '%s'",
                                      result->type_hint.c_str(), path.c_str());
        return result;
    }

    result->managed_object = asset_importer->Import(result.get());
    return result;
}

IAssetPtr AssetDescriptor::ToAssetPtr()
{
    return IAssetPtr::FromAssetDescriptor(shared_from_base<AssetDescriptor>());
}
}