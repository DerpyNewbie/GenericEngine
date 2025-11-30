#pragma once

#include "enable_shared_from_base.h"

namespace engine
{
class Object : public enable_shared_from_base<Object>
{
    friend class Engine;
    friend struct AssetDescriptor;
    friend cereal::access;

    inline static bool m_in_gc_time_;
    inline static unsigned int m_last_instantiated_name_count_;
    inline static unsigned int m_last_immediately_destroyed_objects_;
    inline static std::unordered_map<xg::Guid, std::shared_ptr<Object>> m_objects_;
    inline static std::vector<std::weak_ptr<Object>> m_deserialized_objects_;

    xg::Guid m_guid_;
    bool m_is_destroying_ = false;
    std::string m_name_ = "Unknown Object";

    static void GarbageCollect();
    static void InvokeOnDeserialized();
    static std::string GenerateName();
    static xg::Guid GenerateGuid();

    void SetGuid(xg::Guid new_guid);

public:
    Object() = default;
    virtual ~Object() = default;

    virtual void OnConstructed()
    { }

    virtual void OnDeserialized()
    { }

    virtual void OnDestroy()
    { }

    [[nodiscard]] xg::Guid Guid() const;

    [[nodiscard]] std::string Name() const;
    void SetName(const std::string &name);

    [[nodiscard]] bool IsDestroying() const;
    void DestroyThis();

    static void Destroy(const std::shared_ptr<Object> &obj);
    static void DestroyImmediate(const std::shared_ptr<Object> &obj);

    [[nodiscard]] static std::shared_ptr<Object> Find(const xg::Guid &guid);

    template <class T>
    [[nodiscard]] static std::vector<std::shared_ptr<T>> FindByType()
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");
        std::vector<std::shared_ptr<T>> result;
        for (auto &obj : m_objects_ | std::views::values)
        {
            auto casted_obj = std::dynamic_pointer_cast<T>(obj);
            if (casted_obj != nullptr)
            {
                result.push_back(casted_obj);
            }
        }

        return result;
    }

    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name, const xg::Guid &guid)
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");
        auto obj = std::make_shared<T>();
        auto ptr = std::dynamic_pointer_cast<Object>(obj);
        ptr->SetName(name);
        ptr->m_guid_ = guid;

        m_objects_[guid] = obj;

        ptr->OnConstructed();
        return obj;
    }

    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name)
    {
        return Instantiate<T>(name, GenerateGuid());
    }

    template <class T>
    static std::shared_ptr<T> Instantiate(const xg::Guid &guid)
    {
        return Instantiate<T>("Unnamed Object", guid);
    }

    template <class T>
    static std::shared_ptr<T> Instantiate()
    {
        return Instantiate<T>(GenerateName(), GenerateGuid());
    }

    static std::shared_ptr<Object> Instantiate(const std::shared_ptr<Object> &original);

    template <class Archive>
    void serialize(Archive &ar)
    {
        bool was_just_deserialized = false;
        if constexpr (Archive::is_loading::value)
        {
            was_just_deserialized = m_guid_ == xg::Guid() && m_name_ == "Unknown Object";
        }

        ar(CEREAL_NVP(m_guid_), CEREAL_NVP(m_name_));

        if constexpr (Archive::is_loading::value)
        {
            m_objects_[m_guid_] = shared_from_this();
            if (was_just_deserialized)
            {
                m_deserialized_objects_.emplace_back(shared_from_this());
            }
        }
    }

    [[nodiscard]] bool Equals(const Object *rhs) const
    {
        return Equals(this, rhs);
    }

    [[nodiscard]] static bool Equals(const Object *a, const Object *b)
    {
        return a == b || (a != nullptr && b != nullptr && a->Guid() == b->Guid());
    }
};
}