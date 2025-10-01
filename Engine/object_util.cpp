#include "pch.h"
#include "game_object.h"
#include "scene.h"
#include "Asset/asset_ptr.h"
#include "object_util.h"

namespace engine
{
namespace
{
std::string DeduplicatedName(const std::shared_ptr<GameObject> &object,
                             const std::vector<std::shared_ptr<GameObject>> &siblings)
{
    auto [original_name, dedupe_index] = ObjectUtil::GetOriginalName(object->Name());
    const auto count = std::ranges::count_if(siblings, [original_name](auto a) {
        return ObjectUtil::GetOriginalName(a->Name()).first != original_name;
    });

    if (count == 0)
        return original_name;

    return original_name + " (" + std::to_string(count) + ")";
}
}

std::string ObjectUtil::GetDeduplicatedName(const std::shared_ptr<Object> &object)
{
    auto original_name = object->Name();

    const auto go = std::dynamic_pointer_cast<GameObject>(object);
    if (go != nullptr)
    {
        const auto transform = go->Transform();
        const auto parent = transform->Parent();
        if (parent != nullptr)
        {
            const auto child_count = parent->ChildCount();
            auto vec = std::vector<std::shared_ptr<GameObject>>();
            vec.reserve(child_count);
            for (auto i = 0; i < child_count; ++i)
            {
                auto sibling_go = transform->GetChild(i)->GameObject();
                if (sibling_go != nullptr && sibling_go != go)
                {
                    vec.emplace_back(sibling_go);
                }
            }

            return DeduplicatedName(go, vec);
        }

        auto siblings = go->Scene()->m_root_game_objects_;
        erase_if(siblings, [&go](auto a) {
            return a == go;
        });

        return DeduplicatedName(go, go->Scene()->m_root_game_objects_);
    }

    return original_name;
}

std::pair<std::string, int> ObjectUtil::GetOriginalName(const std::string &name)
{
    const auto pos = name.find_last_of(" (");
    if (pos == std::string::npos)
        return {name, 0};

    return {name.substr(0, pos), std::stoi(name.substr(pos + 2, name.size() - pos - 3))};
}

#pragma push_macro("GetObject")
#undef GetObject
std::string ObjectUtil::MakeClone(const std::string &object_json)
{
    rapidjson::Document document;
    document.Parse(object_json.c_str());

    if (!document.IsObject())
    {
        throw std::runtime_error("Invalid JSON representation of object");
    }

    auto obj = document.GetObject();
    auto engine_objects = FindMatchingObjects(obj, IsEngineObject);
    auto asset_ptrs = FindMatchingObjects(obj, IsAssetPtr);

    std::unordered_map<xg::Guid, xg::Guid> cloned_guid_mapping;
    for (const auto &engine_object : engine_objects)
    {
        auto guid_member = engine_object.FindMember("m_guid_");
        const auto original_guid = xg::Guid(guid_member->value.GetString());
        const auto cloned_guid = xg::newGuid();
        cloned_guid_mapping[original_guid] = cloned_guid;

        const auto cloned_guid_str = cloned_guid.str();
        guid_member->value.SetString(cloned_guid_str.c_str(), document.GetAllocator());
    }

    for (const auto &asset_ptr : asset_ptrs)
    {
        auto guid_member = asset_ptr.FindMember("m_guid_");
        const auto original_guid = xg::Guid(guid_member->value.GetString());
        auto pos = cloned_guid_mapping.find(original_guid);
        if (pos == cloned_guid_mapping.end())
            continue;

        const auto cloned_guid = pos->second;
        const auto cloned_guid_str = cloned_guid.str();
        guid_member->value.SetString(cloned_guid_str.c_str(), document.GetAllocator());
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);
    Logger::Log(buffer.GetString());
    return buffer.GetString();
}

bool ObjectUtil::IsEngineObject(const rapidjson::Document::Object &object)
{
    return object.MemberCount() == 2 &&
           object.HasMember("m_guid_") &&
           object.HasMember("m_name_");
}

bool ObjectUtil::IsAssetPtr(const rapidjson::Document::Object &object)
{
    return object.MemberCount() == 3 &&
           object.HasMember("m_guid_") &&
           object.HasMember("m_type_") &&
           object.HasMember("m_stored_reference_");
}

std::vector<rapidjson::Document::Object> ObjectUtil::FindMatchingObjects(
    const rapidjson::Document::Object &object, const std::function<bool(rapidjson::Document::Object)> &pred)
{
    std::vector<rapidjson::Document::Object> matching_object;
    std::queue<rapidjson::Document::Object> to_process;

    // Start with the root value
    to_process.push(object);

    while (!to_process.empty())
    {
        const auto current = to_process.front();
        to_process.pop();

        // Check if object matches pred function
        if (pred(current))
        {
            matching_object.emplace_back(current);
        }

        // Add all value members to the queue for processing
        for (auto i = current.MemberBegin(); i != current.MemberEnd(); ++i)
        {
            if (i->value.IsObject())
            {
                to_process.push(i->value.GetObject());
            }
            else if (i->value.IsArray())
            {
                for (auto j = i->value.Begin(); j != i->value.End(); ++j)
                {
                    if (j->IsObject())
                        to_process.push(j->GetObject());
                }
            }
        }
    }

    return matching_object;
}
}

#pragma pop_macro("GetObject")