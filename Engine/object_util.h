#pragma once

namespace engine
{
// class SerializedObjectProxy
// {
//     rapidjson::Document::Object &m_json_object_;
//
// public:
//     SerializedObjectProxy(rapidjson::Document::Object &obj) : m_json_object_(obj)
//     {}
//
//     xg::Guid GetGuid() const
//     {
//         return xg::Guid(m_json_object_.FindMember("m_guid_")->value.GetString());
//     }
//
//     void SetGuid(xg::Guid guid) const
//     {
//         auto guid_str = guid.str();
//         m_json_object_.FindMember("m_guid_")->value.SetString(guid_str.c_str(), guid_str.size());
//     }
//
//     std::string GetName() const
//     {
//         return m_json_object_.FindMember("m_name_")->value.GetString();
//     }
//
//     void SetName(const std::string &name) const
//     {
//         m_json_object_.FindMember("m_name_")->value.SetString(name.c_str(), name.size());
//     }
// };
//
// class SerializedAssetPtrProxy
// {
//     rapidjson::Document::Object &m_json_object_;
//
// public:
//     SerializedAssetPtrProxy(rapidjson::Document::Object &obj) : m_json_object_(obj)
//     {}
//
//     xg::Guid GetGuid() const
//     {
//         return xg::Guid(m_json_object_.FindMember("m_guid_")->value.GetString());
//     }
//
//     void SetGuid(xg::Guid guid) const
//     {
//         auto guid_str = guid.str();
//         m_json_object_.FindMember("m_guid_")->value.SetString(guid_str.c_str(), guid_str.size());
//     }
//
//     AssetPtrType GetAssetPtrType() const
//     {
//         return static_cast<AssetPtrType>(m_json_object_.FindMember("m_type_")->value.GetInt());
//     }
//
//     void SetAssetPtrType(AssetPtrType type) const
//     {
//         m_json_object_.FindMember("m_type_")->value.SetInt(static_cast<int>(type));
//     }
// };

class ObjectUtil
{
public:
    static std::string GetDeduplicatedName(const std::shared_ptr<Object> &object);
    static std::pair<std::string, int> GetOriginalName(const std::string &name);
    static std::string MakeClone(const std::string &object_json);
    static bool IsEngineObject(const rapidjson::Document::Object &object);
    static bool IsAssetPtr(const rapidjson::Document::Object &object);
    static std::vector<rapidjson::Document::Object> FindMatchingObjects(const rapidjson::Document::Object &object,
                                                                        const std::function<bool(
                                                                            rapidjson::Document::Object)> &pred);

};
}