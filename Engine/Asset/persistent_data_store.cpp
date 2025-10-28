#include "pch.h"

#include "persistent_data_store.h"

namespace engine
{
using namespace rapidjson;

#pragma push_macro("GetObject")
#undef GetObject

auto PersistentDataStore::Find(const std::string &key) const
{
    return m_value_->FindMember(StringRef(key.c_str(), key.size()));
}
void PersistentDataStore::Set(const std::string &key, Value &value) const
{
    auto json_key = Value(key.c_str(), static_cast<SizeType>(key.size()), Allocator());
    const auto it = m_value_->FindMember(key.c_str());
    if (it == m_value_->MemberEnd())
    {
        m_value_->AddMember(json_key, value, Allocator());
        return;
    }

    it->value = value;
}

MemoryPoolAllocator<> &PersistentDataStore::Allocator() const
{
    return m_document_->GetAllocator();
}

bool PersistentDataStore::HasKey(const std::string &key) const
{
    return m_value_->HasMember(key.c_str());
}

void PersistentDataStore::RemoveKey(const std::string &key) const
{
    m_value_->RemoveMember(key.c_str());
}

std::vector<std::string> PersistentDataStore::GetKeys() const
{
    std::vector<std::string> keys;
    for (auto i = m_value_->MemberBegin(); i != m_value_->MemberEnd(); ++i)
    {
        keys.emplace_back(i->name.GetString());
    }

    return keys;
}

void PersistentDataStore::ClearKeys() const
{
    auto data = m_value_->GetObject();
    data.RemoveAllMembers();
}

void PersistentDataStore::SetString(const std::string &key, const std::string &value) const
{
    auto json_value = Value(value.c_str(), static_cast<SizeType>(value.size()), Allocator());
    Set(key, json_value);
}

void PersistentDataStore::SetInt(const std::string &key, const int value) const
{
    auto json_value = Value(value);
    Set(key, json_value);
}

void PersistentDataStore::SetFloat(const std::string &key, const float value) const
{
    auto json_value = Value(value);
    Set(key, json_value);
}

void PersistentDataStore::SetBool(const std::string &key, const bool value) const
{
    auto json_value = Value(value);
    Set(key, json_value);
}

void PersistentDataStore::SetDataStore(const std::string &key, const PersistentDataStore &value) const
{
    const auto it = Find(key);
    if (it == m_value_->MemberEnd())
    {
        auto json_value = Value(kObjectType);
        json_value.CopyFrom(*value.m_value_, Allocator());
        m_value_->AddMember(StringRef(key.c_str(), key.size()), Value(kObjectType), Allocator());
        return;
    }

    if (!it->value.IsObject())
    {
        it->value.SetObject();
    }

    if (it->value == *value.m_value_)
    {
        return;
    }

    it->value.CopyFrom(*value.m_value_, Allocator());
}

void PersistentDataStore::SetValue(const std::string &key, Value &value) const
{
    Set(key, value);
}

std::string PersistentDataStore::GetString(const std::string &key) const
{
    const auto json_value = Find(key);
    if (json_value == m_value_->MemberEnd() || !json_value->value.IsString())
    {
        return "";
    }

    return json_value->value.GetString();
}

int PersistentDataStore::GetInt(const std::string &key) const
{
    const auto json_value = Find(key);
    if (json_value == m_value_->MemberEnd())
    {
        return 0;
    }

    return json_value->value.GetInt();
}

float PersistentDataStore::GetFloat(const std::string &key) const
{
    const auto json_value = Find(key);
    if (json_value == m_value_->MemberEnd())
    {
        return 0.0f;
    }

    return json_value->value.GetFloat();
}

bool PersistentDataStore::GetBool(const std::string &key) const
{
    const auto json_value = Find(key);
    if (json_value == m_value_->MemberEnd())
    {
        return false;
    }

    return json_value->value.GetBool();
}

PersistentDataStore PersistentDataStore::GetDataStore(const std::string &key) const
{
    const auto json = GetString(key);
    const auto it = m_value_->FindMember(StringRef(key.c_str(), key.size()));

    if (!it->value.IsObject())
    {
        it->value.SetObject();
    }

    const PersistentDataStore data_store{m_document_, &it->value};
    return data_store;
}

Value &PersistentDataStore::GetValue(const std::string &key) const
{
    return Find(key)->value;
}

bool PersistentDataStore::IsString(const std::string &key) const
{
    const auto json_value = Find(key);
    return json_value != m_value_->MemberEnd() && json_value->value.IsString();
}

bool PersistentDataStore::IsInt(const std::string &key) const
{
    const auto json_value = Find(key);
    return json_value != m_value_->MemberEnd() && json_value->value.IsInt();
}

bool PersistentDataStore::IsFloat(const std::string &key) const
{
    const auto json_value = Find(key);
    return json_value != m_value_->MemberEnd() && json_value->value.IsFloat();
}

bool PersistentDataStore::IsBool(const std::string &key) const
{
    const auto json_value = Find(key);
    return json_value != m_value_->MemberEnd() && json_value->value.IsBool();
}

bool PersistentDataStore::IsDataStore(const std::string &key) const
{
    const auto json_value = Find(key);
    return json_value != m_value_->MemberEnd() && json_value->value.IsObject();
}

#pragma pop_macro("GetObject")
}