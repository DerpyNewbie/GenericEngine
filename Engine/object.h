#pragma once

#include "enable_shared_from_base.h"

namespace engine
{
class Object : public enable_shared_from_base<Object>
{
    friend class Engine;
    friend struct AssetDescriptor;
    friend cereal::access;

    static bool m_in_gc_time_;
    static unsigned int m_last_instantiated_name_count_;
    static unsigned int m_last_immediately_destroyed_objects_;
    static std::unordered_map<xg::Guid, std::shared_ptr<Object>> m_objects_;
    static std::vector<std::shared_ptr<Object>> m_destroying_objects_;
    static std::vector<std::shared_ptr<Object>> m_destroyed_objects_;

    xg::Guid m_guid_;
    bool m_is_destroying_ = false;
    std::string m_name_ = "Unknown Object";

    static void GarbageCollect();
    static std::string GenerateName();
    static xg::Guid GenerateGuid();

    void SetGuid(xg::Guid new_guid);

protected:
    Object() = default;

public:
    virtual ~Object() = default;

    virtual void OnConstructed()
    {}

    virtual void OnDestroy()
    {}

    xg::Guid Guid() const;

    std::string Name() const;
    void SetName(const std::string &name);

    bool IsDestroying() const;
    void DestroyThis();

    static void Destroy(const std::shared_ptr<Object> &obj);
    static void DestroyImmediate(const std::shared_ptr<Object> &obj);

    static std::shared_ptr<Object> Find(const xg::Guid &guid);

    template <class T>
    static std::vector<std::shared_ptr<T>> FindByType()
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

    template <class Archive>
    void serialize(Archive &ar)
    {
        bool is_loading = m_guid_ == xg::Guid() && m_name_ == "";
        ar(CEREAL_NVP(m_guid_), CEREAL_NVP(m_name_));

        if (is_loading)
        {
            m_objects_[m_guid_] = shared_from_this();
        }
    }

    bool Equals(const Object *rhs) const
    {
        return Equals(this, rhs);
    }

    static bool Equals(const Object *a, const Object *b)
    {
        return a == b || (a != nullptr && b != nullptr && a->Guid() == b->Guid());
    }
};
}