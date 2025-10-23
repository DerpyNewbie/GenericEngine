#pragma once

namespace engine
{
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