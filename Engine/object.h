#pragma once
#include <memory>
#include <string>

#include "enable_shared_from_base.h"

namespace engine
{
class Object : public enable_shared_from_base<Object>
{
    friend class Engine;

    static unsigned int m_last_immediately_destroyed_objects_;
    static std::vector<std::shared_ptr<Object>> m_destroying_objects_;

    bool m_is_destroying_ = false;
    std::string m_name_ = "Unknown Object";

    static void DestroyObjects();

protected:
    Object() = default;

public:
    virtual ~Object() = default;

    virtual void OnConstructed()
    {}

    virtual void OnDestroy()
    {}

    std::string Name() const;
    void SetName(const std::string &name);

    bool IsDestroying() const;
    void DestroyThis();

    static void Destroy(const std::shared_ptr<Object> &obj);
    static void DestroyImmediate(const std::shared_ptr<Object> &obj);

    template <class T>
    static std::shared_ptr<T> Instantiate()
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");
        auto obj = std::make_shared<T>();
        std::dynamic_pointer_cast<Object>(obj)->OnConstructed();
        return obj;
    }

    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name)
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");
        auto obj = std::make_shared<T>();
        auto ptr = std::dynamic_pointer_cast<Object>(obj);
        ptr->SetName(name);
        ptr->OnConstructed();
        return obj;
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(m_name_));
    }
};
}