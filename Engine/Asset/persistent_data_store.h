#pragma once
#include <string>
#include <vector>

namespace engine
{
class PersistentDataStore
{
    rapidjson::Document *m_document_;
    rapidjson::Value *m_value_;

    [[nodiscard]] auto Find(const std::string &key) const;
    void Set(const std::string &key, rapidjson::Value &value) const;
    [[nodiscard]] rapidjson::MemoryPoolAllocator<> &Allocator() const;

public:
    explicit PersistentDataStore(rapidjson::Document *document, rapidjson::Value *value) :
        m_document_(document), m_value_(value)
    {}

    [[nodiscard]] bool HasKey(const std::string &key) const;
    void RemoveKey(const std::string &key) const;
    [[nodiscard]] std::vector<std::string> GetKeys() const;
    void ClearKeys() const;

    void SetString(const std::string &key, const std::string &value) const;
    void SetInt(const std::string &key, int value) const;
    void SetFloat(const std::string &key, float value) const;
    void SetBool(const std::string &key, bool value) const;
    void SetDataStore(const std::string &key, const PersistentDataStore &value) const;
    void SetValue(const std::string &key, rapidjson::Value &value) const;

    [[nodiscard]] std::string GetString(const std::string &key) const;
    [[nodiscard]] int GetInt(const std::string &key) const;
    [[nodiscard]] float GetFloat(const std::string &key) const;
    [[nodiscard]] bool GetBool(const std::string &key) const;
    [[nodiscard]] PersistentDataStore GetDataStore(const std::string &key) const;
    [[nodiscard]] rapidjson::Value &GetValue(const std::string &key) const;

    [[nodiscard]] bool IsString(const std::string &key) const;
    [[nodiscard]] bool IsInt(const std::string &key) const;
    [[nodiscard]] bool IsFloat(const std::string &key) const;
    [[nodiscard]] bool IsBool(const std::string &key) const;
    [[nodiscard]] bool IsDataStore(const std::string &key) const;

    template <typename Writer>
    void Write(Writer &writer) const
    {
        m_value_->Accept(writer);
    }
};
}