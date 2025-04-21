#pragma once
#include <memory>
#include <string>

#include "enable_shared_from_base.h"

namespace engine
{
class Object : public enable_shared_from_base<Object>
{
public:
    virtual ~Object() = default;

    std::string GetName()
    {
        return m_name_;
    }

    virtual void SetName(const std::string &name)
    {
        m_name_ = name;
    }

    template <class T>
    static std::shared_ptr<T> Instantiate()
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");
        return std::make_shared<T>();
    }

    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name)
    {
        auto o = Instantiate<T>();
        o->SetName(name);
        return o;
    }

protected:
    std::string m_name_ = "Unknown Object";

    Object() = default;
};
}