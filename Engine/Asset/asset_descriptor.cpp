#include "pch.h"

#include "asset_descriptor.h"

#include "asset_database.h"
#include "logger.h"
#include "Importer/asset_importer.h"

namespace engine
{
using namespace rapidjson;

path AssetDescriptor::AssetFilePath(const path &asset_path)
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

path AssetDescriptor::MetaFilePath(const path &asset_path)
{
    const auto absolute_asset_path = AssetFilePath(asset_path);
    return absolute_asset_path.string() + kMetaFileExtension;
}

bool AssetDescriptor::GetMetaJson(const path &asset_path, std::string &out_json)
{
    const auto descriptor_file_path = MetaFilePath(asset_path);
    if (!exists(descriptor_file_path))
    {
        return false;
    }

    auto input_stream = std::ifstream(descriptor_file_path);
    out_json = std::string(std::istreambuf_iterator(input_stream), std::istreambuf_iterator<char>());
    return true;
}

void AssetDescriptor::WriteMeta(const path &path)
{
    m_meta_data_store_.SetString(kGuidKey, m_guid_.str());
    m_meta_data_store_.SetString(kTypeKey, m_type_);
    m_meta_data_store_.SetDataStore(kDataKey, m_user_data_store_);

    Value guids;
    guids.SetArray();
    if (m_objects_.size() > 1)
    {
        // off by one because the main object is also present in the objects
        for (auto it = std::next(m_objects_.begin(), 1); it != m_objects_.end(); ++it)
        {
            auto value = (*it)->Guid().str();
            auto object_guid = Value(value.c_str(), static_cast<SizeType>(value.size()), m_meta_json_.GetAllocator());
            guids.PushBack(object_guid, m_meta_json_.GetAllocator());
        }
    }

    m_meta_data_store_.SetValue(kSubGuidsKey, guids);

    StringBuffer sb;
    PrettyWriter writer(sb);
    m_meta_json_.Accept(writer);

    auto file_output = std::ofstream(path);
    file_output << sb.GetString();
    file_output.flush();
    file_output.close();
}

AssetDescriptor::AssetDescriptor(const path &file_path) :
    m_type_(file_path.extension().string()), m_asset_path_(file_path),
    m_meta_data_store_(nullptr, nullptr), m_user_data_store_(nullptr, nullptr)
{
    std::string meta_json;
    const std::string meta_file_path = MetaFilePath(file_path).string();
    const path asset_file_path = AssetFilePath(file_path);

    m_asset_path_ = asset_file_path;
    m_type_ = file_path.extension().string();

    m_meta_json_ = Document();

    if (!GetMetaJson(file_path, meta_json))
    {
        Logger::Log<AssetDescriptor>("No meta file `%s` found. Generating!", file_path.string().c_str());
        m_guid_ = xg::newGuid();
        m_meta_json_.SetObject();
        m_meta_data_store_ = PersistentDataStore{&m_meta_json_, &m_meta_json_};
        auto data_value = Value(kObjectType);
        m_meta_data_store_.SetValue(kDataKey, data_value);
        m_user_data_store_ = m_meta_data_store_.GetDataStore(kDataKey);

        WriteMeta(meta_file_path);

        if (!GetMetaJson(file_path, meta_json))
        {
            Logger::Error<AssetDescriptor>("Failed to generate meta file for asset '%s'!", file_path.string().c_str());
            assert(false);
        }
    }

    m_meta_json_ = Document();
    m_meta_json_.Parse(meta_json.c_str());

    m_meta_data_store_ = PersistentDataStore{&m_meta_json_, &m_meta_json_};
    m_user_data_store_ = m_meta_data_store_.GetDataStore(kDataKey);
    m_type_ = m_meta_data_store_.GetString(kTypeKey);
    m_guid_ = xg::Guid(m_meta_data_store_.GetString(kGuidKey));

    // load sub guids
    auto &objects = m_meta_data_store_.GetValue(kSubGuidsKey);
    if (!objects.IsArray())
    {
        Logger::Warn<AssetDescriptor>("No objects found in meta file for asset '%s'!", m_asset_path_.string().c_str());
        objects.SetArray();
    }

    const auto array = objects.GetArray();
    for (auto i = array.begin(); i != array.end(); ++i)
        m_sub_guids_.emplace_back(xg::Guid(i->GetString()));
}

xg::Guid AssetDescriptor::Guid() const
{
    return m_guid_;
}

std::list<xg::Guid> AssetDescriptor::SubGuids() const
{
    return m_sub_guids_;
}

path AssetDescriptor::AssetPath() const
{
    return m_asset_path_;
}

std::shared_ptr<Object> AssetDescriptor::MainObject() const
{
    return m_main_object_;
}

std::list<std::shared_ptr<Object>> AssetDescriptor::Objects() const
{
    return m_objects_;
}

PersistentDataStore &AssetDescriptor::DataStore()
{
    return m_user_data_store_;
}

std::list<ImportLog> AssetDescriptor::ImportLogs() const
{
    return m_import_logs_;
}

void AssetDescriptor::SetMainObject(std::shared_ptr<Object> object)
{
    m_main_object_ = object;

    std::erase_if(m_objects_, [&object](auto &item) {
        return item == object;
    });

    m_objects_.emplace_front(object);

    object->SetGuid(m_guid_);
    object->SetName(AssetPath().filename().string());
}

void AssetDescriptor::AddObject(std::shared_ptr<Object> object)
{
    if (m_main_object_ == nullptr)
    {
        SetMainObject(object);
        return;
    }

    m_objects_.emplace_back(object);
    const auto offset = m_objects_.size() - 1;
    if (m_sub_guids_.size() < offset)
    {
        m_sub_guids_.emplace_back(object->Guid());
        return;
    }

    const auto it = std::next(m_sub_guids_.begin(), offset - 1);
    object->SetGuid(*it);
    object->SetName(object->Name() + " (" + AssetPath().filename().string() + ")");
}

void AssetDescriptor::LogImportError(const std::string &message)
{
    m_import_logs_.emplace_back(message, ImportLog::kLogType::kError);
}

void AssetDescriptor::LogImportWarning(const std::string &message)
{
    m_import_logs_.emplace_back(message, ImportLog::kLogType::kWarning);
}

void AssetDescriptor::Import()
{
    m_main_object_ = nullptr;
    m_objects_.clear();

    const auto asset_importer = AssetImporter::Get(m_type_);
    if (asset_importer == nullptr)
    {
        Logger::Warn<AssetDescriptor>("Asset importer for type '%s' not found! Will ignore file '%s'",
                                      m_type_.c_str(), AssetPath().string().c_str());
        return;
    }

    asset_importer->OnImport(this);
    Save();
}

void AssetDescriptor::Save()
{
    if (IsInternalAsset())
    {
        Logger::Warn<AssetDescriptor>("Cannot save internal asset '%s'", m_guid_.str().c_str());
        return;
    }

    WriteMeta(MetaFilePath(AssetPath()));
}

bool AssetDescriptor::IsInternalAsset() const
{
    return AssetPath().string() == kInternalAssetPath;
}
}